#include <cc/asm/asm_file.h>

#include <cc/x86/instruction.h>
#include <cc/x86/gp_register.h>
#include <cc/coff/section.h>
#include <cc/timing.h>

using namespace cc::assembly;

struct parser_state {
	asm_parser* parser;
	token current_token;
};

// #todo (bwilks) -> FIX. We need some better way of accessing file text
// This is needed as error can occur during code generation (based off the AST)
// and it is useful to have line numbers and so forth pointing to the offending
// instruction
// 
// Possible fix -> implement semantic validation stage - after AST has been created
// go over the AST and check if it makes sense, etc the instruction exists, it's operands are in the
// right order, etc...
// 
// NB: to dev -> this is set just before the AST is generated and is valid for the duration of the 
// ast_file constructor. <-- BIGGIE, here peeps. don't forget this.
static cc::string *s_file_text = nullptr;

static void error_at_tok(const token& tok, const cc::string& msg) {
	emit_parse_error(tok.get_line(), tok.get_col(), tok.get_index(), *s_file_text, msg);
}

static void error_here(parser_state& parser, const cc::string& msg) {
	token& cur_tok = parser.current_token;
	emit_parse_error(cur_tok.get_line(), cur_tok.get_col(), cur_tok.get_index(), parser.parser->get_text(), msg);
}

static void advance(parser_state& parser) {
	parser.current_token = parser.parser->parse_next_token();
}

static cc::string get_token_string(const token& tok) {
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

static bool is_register(const cc::string& s) {
	return s_reg_map.find(s) != s_reg_map.end();
}

static ast_node* parse_instruction(parser_state& p_state) {
	const token id_tok = p_state.current_token;
	
	advance(p_state);
	
	ast_instruction* ins = new ast_instruction(get_token_string(id_tok), nullptr, nullptr);
	ins->tok = id_tok;	
	// Will either be the first operand or a newline (in the event the instruction has no operands)
	const token operand1_tok = p_state.current_token;
	const cc::string operand1_string = get_token_string(operand1_tok);
	
	// The first operand to this instruction is a identifier (not a numeric literal)
	if(accept(p_state, kTok_Identifier)) {
		if(!is_register(operand1_string)) {
			// #todo (bwilks) -> this is only until labels are implemented (to refer to memory locations)
			// need to look into whether instructions even allow reg/memory first operands anyway 
			error_at_tok(operand1_tok, cc::format_string("Register '{0}' does not exist.", operand1_string));
			delete ins;
			return nullptr;
		}
		
		// #todo (bwilks) This assumes that the first operand is going to be a register (could be a label?) fix.
		ins->first_operand = new ast_register(operand1_string);

		if(accept(p_state, kTok_Comma)) {
			// Cache the second operands token, as the following `advance` calls
			// will change the parse states current token so that it no longer represents 
			// the second operand
			const token operand2_tok = p_state.current_token;
			const cc::string operand2_string = get_token_string(operand2_tok);

			if(accept(p_state, kTok_IntLiteral)) {
				ins->second_operand = new ast_int_literal(std::stoi(operand2_string));
			} else if(accept(p_state, kTok_Identifier)) {
				// See if the second operand is a register or a label
				const bool is_register = is_register(operand2_string);
				
				if(is_register) {
					ins->second_operand = new ast_register(operand2_string);
				} else {
					// #todo (bwilks) -> fix (remove when labels & memory addressing is implemented)
					// see previous todo
					error_at_tok(operand2_tok, "Label (memory address) operands are not currently supported!");
					delete ins;
					return nullptr;
					// If the second operand is not a register then it will probably be a label.
					// ins->second_operand = new ast_label(operand2_string);
				}
			} else {
				delete ins;
				error_here(p_state, "Oi, there's an invalid second instruction operand over 'ere!");
				return nullptr;
			}
		}
	} else if(accept(p_state, kTok_IntLiteral)) { // The first operand to this instruction is a numeric literal
		ins->first_operand = new ast_int_literal(std::stoi(operand1_string));
	} else if(p_state.current_token.get_type() != kTok_Newline) {
		// If the has not been parsed (and is not a newline) then error, cus' we have no idea what it is :D
		delete ins;
		error_here(p_state, "Invalid first operand! :D");
		return nullptr;
	}
	
	// Instructions should finish with a newline
	if(!expect_cur(p_state, kTok_Newline)) {
		delete ins;
		return nullptr;
	}

	return ins;
}

cc::array<ast_section*> asm_file::gen_ast(asm_parser& parser) {
	time_block perf([&](float s){
		CINFO("Parsed {0} lines in {1}s", parser.get_lines_parsed(), s);
	});
	
	parser_state p_state;
	p_state.parser = &parser;
	p_state.current_token = parser.parse_next_token();
	
	s_file_text = &parser.get_text();

	cc::array<ast_section*> sections;
	bool stop_parsing = false;

	for(;;) {
		token& ctok = p_state.current_token;
		switch(ctok.get_type()) {
		case kTok_EOF:
		{
			stop_parsing = true;
			break;
		}
		case kTok_Newline:
		{
			break;
		}
		case kTok_Identifier:
		{
			ast_node* node = parse_instruction(p_state);
			
			// there has been a parse error occur while parsing the instruction
			// so stop parsing
			if(!node) {
				stop_parsing = true;
				break;
			}

			ast_section* current_section = sections.back();
			current_section->nodes.push_back(node);

			break;
		}	
		case kTok_Directive:
		{
			// At the moment we only parse '.section' directives.
			// And since section names start with a '.' they are classed as directives
			// hence why we accept a directive here
			if(accept(p_state, kTok_Directive)) {
				ast_section* sec = new ast_section(get_token_string(p_state.current_token));
				sections.push_back(sec);
			}

			break;
		}}

		if(stop_parsing) {
			break;
		}

		p_state.current_token = parser.parse_next_token();
	}
	
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
					error_at_tok(tok, cc::format_string("Invalid instruction operands for '{0}'", ins_name));

					return instruction::make_op(kNop);
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

	error_at_tok(tok, cc::format_string("No such instruction '{0}'", ins_name));
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

