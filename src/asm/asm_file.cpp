#include <cc/asm/asm_file.h>

#include <cc/x86/instruction.h>
#include <cc/x86/gp_register.h>
#include <cc/coff/section.h>
#include <cc/timing.h>

using namespace cc::assembly;

struct parser_state {
	asm_parser* parser;
	token current_token;
	cc::array<ast_section*> *sections;
};

static void error_here(parser_state& parser, const cc::string& msg) {
	token& cur_tok = parser.current_token;
	emit_parse_error(cur_tok.get_line(), cur_tok.get_col(), cur_tok.get_index(), parser.parser->get_text(), msg);
}

static void advance(parser_state& parser) {
	parser.current_token = parser.parser->parse_next_token();
}

static cc::string get_token_string(token& tok) {
	cc::string tok_str(tok.get_data(), tok.get_data() + tok.get_data_length());
	return tok_str;
}

// Returns true if the current token is of the correct type
// false & error message otherwise.
static bool expect_cur(parser_state& parser, token_type expected_type) {
	if(parser.current_token.get_type() != expected_type) {
		cc::string tok_str = get_token_string(parser.current_token);
		error_here(parser, cc::format_string("Unexpected token '{0}'.", tok_str));
		return false;
	}	
	return true;
}

static bool accept(parser_state& parser, token_type expected) {
	if(parser.current_token.get_type() == expected) {
		advance(parser);
		return true;
	}
	return false;
}

static std::unordered_map<cc::string, const cc::x86::gp_register*> s_reg_map = {
	{ "eax", &cc::x86::gp_register::eax },
	{ "ebx", &cc::x86::gp_register::ebx },
	{ "ecx", &cc::x86::gp_register::ecx },
	{ "edx", &cc::x86::gp_register::edx },
	{ "esp", &cc::x86::gp_register::esp },
	{ "ebp", &cc::x86::gp_register::ebp },
	{ "esi", &cc::x86::gp_register::esi },
	{ "edi", &cc::x86::gp_register::edi }
};


ast_register::ast_register(cc::string reg_name) {
	type = kAst_Register;
	reg = s_reg_map[reg_name];
}

static ast_node* parse_instruction(parser_state& p_state) {
	token id_tok = p_state.current_token;
	
	advance(p_state);
	
	ast_instruction* ins = new ast_instruction(get_token_string(id_tok), nullptr, nullptr);
	
	// Will either be the first operand or a newline (in the event the instruction has no operands)
	token operand1_tok = p_state.current_token;
	cc::string operand1_string = get_token_string(operand1_tok);
	
	if(accept(p_state, kTok_Identifier)) {
		// #todo (bwilks) This assumes that the first operand is going to be a register (could be a label?) fix.
		ins->first_operand = new ast_register(operand1_string);

		if(accept(p_state, kTok_Comma)) {
			token operand2_tok = p_state.current_token;
			cc::string operand2_string = get_token_string(operand2_tok);

			if(accept(p_state, kTok_IntLiteral)) {
				ins->second_operand = new ast_int_literal(std::stoi(operand2_string));
			} else if(accept(p_state, kTok_Identifier)) {
				auto it = s_reg_map.find(operand2_string);
				bool is_register = it != s_reg_map.end();
				
				if(is_register) {
					ins->second_operand = new ast_register(operand2_string);
				} else {
					ins->second_operand = new ast_label(operand2_string);
				}
			} else {
				error_here(p_state, "Oi, there's an invalid second instruction operand over 'ere!");
				return nullptr;
			}
		}
	} else if(accept(p_state, kTok_IntLiteral)) {
		ins->first_operand = new ast_int_literal(std::stoi(operand1_string));
	}

	if(!expect_cur(p_state, kTok_Newline))
		return nullptr;

	return ins;
}

cc::array<ast_section*> asm_file::gen_ast(asm_parser& parser) {
	time_block perf([&](float s){
		CINFO("Parsed {0} lines in {1}s", parser.get_lines_parsed(), s);
	});
	
	parser_state p_state;
	p_state.parser = &parser;
	p_state.current_token = parser.parse_next_token();

	cc::array<ast_section*> sections;
	
	p_state.sections = &sections;

	for(;;) {
		token& ctok = p_state.current_token;
		switch(ctok.get_type()) {
		case kTok_EOF:
			goto end; // yeah, it's evil, shoot me
		case kTok_Newline:
			break;
		case kTok_Identifier:
		/*{
			token id_tok = p_state.current_token;
			advance(p_state);
			
			ast_instruction* ins = new ast_instruction(get_token_string(id_tok), nullptr, nullptr);
			token first_tok = p_state.current_token;
			// first operand
			if(accept(p_state, kTok_Identifier)) {
				ins->first_operand = new ast_register(get_token_string(first_tok));
				if(accept(p_state, kTok_Comma)) {
					token second_tok = p_state.current_token;
					// second operand.
					if(accept(p_state, kTok_IntLiteral)) {
						// whatever...
						ins->second_operand = new ast_int_literal(std::stoi(get_token_string(second_tok)));
					} else if(accept(p_state, kTok_Identifier)) {
						// whatever...
						cc::string tok_str = get_token_string(second_tok);
						auto it = s_reg_map.find(tok_str);
						if(it != s_reg_map.end()) {
							ins->second_operand = new ast_register(tok_str);
						} else {
							ins->second_operand = new ast_label(tok_str);
						}
					}
					else {
						error_here(p_state, "Invalid second operand to instruction");
						goto end; // again, evil
					}
				}
			} else if(accept(p_state, kTok_IntLiteral)) {
				ins->first_operand = new ast_int_literal(std::stoi(get_token_string(first_tok)));
			} else if(p_state.current_token.get_type() == kTok_Newline) {
			}
			
			ast_section* cur_section = sections.back();
			cur_section->nodes.push_back(ins);

			if(!expect_cur(p_state, kTok_Newline))
				goto end; // whatever
			break;
		}*/
		{
			ast_node* node = parse_instruction(p_state);
			
			if(!node)
				goto end;

			ast_section* current_section = sections.back();
			current_section->nodes.push_back(node);

			break;
		}	
		case kTok_Directive:
			if(accept(p_state, kTok_Directive)) {
				ast_section* sec = new ast_section(get_token_string(p_state.current_token));
				sections.push_back(sec);
			}
			break;
		}
		p_state.current_token = parser.parse_next_token();
	}
	end: // *sigh* I KNOW. #todo (bwilks) -> fix the control flow here to get rid of goto
	
	return sections;
}

cc::x86::instruction ast_instruction::gen_x86() {
	using namespace cc::x86;

	for(const auto& opcode_def : instructions_map) {
		instruction_def opcode = opcode_def.second;
		if(std::strcmp(opcode.get_name(), ins_name.c_str()) == 0) {
			instruction_operands_def mode;
			if(first_operand == nullptr && second_operand == nullptr) {
				mode = kInsOps_none;
			} else {
				if(second_operand == nullptr) {
					CFATAL("Cannot generate x86 for unsupported operands mode");
				} else {
					if(second_operand->type == kAst_IntLiteral) {
						mode = kInsOps_rm32imm32;
					} else {
						mode = kInsOps_r32rm32;
					}	
				}
			}

			if(opcode.get_operands_def() == mode) {
				switch(mode) {
				case kInsOps_rm32imm32:
				{
					ast_register* r = (ast_register*)first_operand;
					ast_int_literal* i = (ast_int_literal*)second_operand;
					return instruction::make_reg_imm_op(opcode_def.first, *(r->reg), i->number);
				}
				case kInsOps_r32rm32:
				{
					ast_register* r = (ast_register*)first_operand;
					ast_register* r2 = (ast_register*)second_operand;
					return instruction::make_2reg_op(opcode_def.first, *(r->reg), *(r2->reg));
				}
				case kInsOps_none:
					return instruction::make_op(opcode_def.first);
				}
			}
		}
	}

	CWARN("Cannot generate x86 for '{0}'", ins_name);
	return instruction::make_op(kNop);
}

asm_file::asm_file(const cc::string& filepath) {
	asm_parser parser = asm_parser::from_file(filepath);
	cc::array<ast_section*> sections = gen_ast(parser);

	cc::unique_ptr<coff::object_file> object = cc::make_unique<coff::object_file>();
	{
		cc::time_block perf([](float seconds){
			CINFO("Code generation took {0}s", seconds);
		});
		for(ast_section* section : sections) {
			cc::shared_ptr<cc::coff::section> obj_section = cc::make_shared<cc::coff::section>();
			obj_section->set_name(section->name);
			cc::u32 characteristics = 0;
			if(section->name == ".text") {
				characteristics = cc::coff::kImageScnMemExecute | cc::coff::kImageScnCntCode | cc::coff::kImageScnMemRead;
				obj_section->set_characteristics(characteristics);

				cc::x86::instructions_collection instructions;
				for(ast_node* node : section->nodes) {
					if(node->type != kAst_Instruction) {
						CWARN("Cannot gen x86 for non instruction. Skipping");
						return;
					}

					ast_instruction* instruction = (ast_instruction*)node;
					instructions.add(instruction->gen_x86());
				}
				obj_section->set_raw_data(instructions.combine());
			} else {
				CFATAL("Unknown section aborting");
				return;
			}

			object->add_section(obj_section);
		}
	}

	object->set_machine(cc::coff::kMachinei386);
	object->set_datetime((cc::u32)time(0));
	
	cc::string drectve_str = "/ENTRY:main ";

	cc::shared_ptr<coff::section> drectve = cc::make_shared<coff::section>();
	drectve->set_name(".drectve")
		.set_characteristics(cc::coff::kImageScnLnkRemove | cc::coff::kImageScnLnkInfo)
		.set_raw_data(cc::array<cc::u8>(drectve_str.begin(), drectve_str.end()));

	object->add_section(drectve);
	
	cc::shared_ptr<cc::coff::symbol_table> sym_table = cc::make_shared<cc::coff::symbol_table>();
	
	cc::coff::symbol sym;
	sym.is_aux = false;
	sym.number_of_aux_symbols = 0;
	sym.name = cc::coff::symbol_name("_main");
	sym.section_number = 1;
	sym.storage_clss = cc::coff::kImageSymClassExternal;
	sym.value = 0;
	sym_table->add_symbol(sym);
	object->set_symbol_table(sym_table);

	m_object_file = std::move(object);
}

