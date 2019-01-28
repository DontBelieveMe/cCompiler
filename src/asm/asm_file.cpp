#include <cc/asm/asm_file.h>

#include <cc/x86/instruction.h>
#include <cc/x86/gp_register.h>
#include <cc/coff/section.h>
#include <cc/timing.h>
#include <cc/assert.h>
#include <algorithm>

using namespace cc::assembly;

struct parser_state {
	asm_parser* parser;
	token current_token;
	cc::array<ast_section*> *sections;
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
// asm_file constructor. <-- BIGGIE, here peeps. don't forget this.
static cc::string *s_file_text = nullptr;
static bool s_found_errors = false;

static void error_here(parser_state& parser, const cc::string& msg) {
	token& cur_tok = parser.current_token;
	file_location loc = cur_tok.get_location();
	s_found_errors = true;
	emit_parse_error(loc.line, loc.col, loc.index, parser.parser->get_text(), msg);
}

static void error_at_loc(file_location loc, const cc::string& msg) {
	emit_parse_error(loc.line, loc.col, loc.index, *s_file_text, msg);	
	s_found_errors = true;
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
	if (parser.current_token.get_type() == kTok_Error) {
		return true;
	}

	if(parser.current_token.get_type() != expected_type) {
		cc::string tok_str = get_token_string(parser.current_token);
		error_here(parser, cc::format_string("Unexpected token '{0}'.", tok_str));
		return false;
	}	
	return true;
}

static bool accept(parser_state& parser, token_type expected) {
	if(parser.current_token.get_type() == expected || parser.current_token.get_type() == kTok_Error) {
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
	ins->loc = file_location(id_tok.get_location());	
	
	// Will either be the first operand or a newline (in the event the instruction has no operands)
	const token operand1_tok = p_state.current_token;
	const cc::string operand1_string = get_token_string(operand1_tok);
	
	// The first operand to this instruction is a identifier (not a numeric literal)
	if(accept(p_state, kTok_Identifier)) {
		if(!is_register(operand1_string)) {
			// #todo (bwilks) -> this is only until labels are implemented (to refer to memory locations)
			// need to look into whether instructions even allow reg/memory first operands anyway 
			//error_at_tok(operand1_tok, cc::format_string("Register '{0}' does not exist.", operand1_string));
			//ast_label* label = new ast_label(operand1_string);
			for (ast_section* section : *p_state.sections) {
				for (ast_node* node : section->nodes) {
					if (node->type == kAst_Label) {
						ast_label* l = (ast_label*)node;
						if (l->name == operand1_string) {
							ins->first_operand = l;
						}
					}
				}
			}
			ASSERT(ins->first_operand != nullptr, "Ahhhh, error");
		} else {
			ins->first_operand = new ast_register(operand1_string);
		}

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
				const bool second_operand_is_register = is_register(operand2_string);
				
				if(second_operand_is_register) {
					ins->second_operand = new ast_register(operand2_string);
				} else {
					// #todo (bwilks) -> fix (remove when labels & memory addressing is implemented)
					// see previous todo
					//error_at_tok(operand2_tok, "Label (memory address) operands are not currently supported!");
					error_at_loc(operand2_tok.get_location(), "Labels as operands are not currently supported!");
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
	} else if(accept(p_state, kTok_Colon)) {
		// It's a label!
		
		delete ins;
		
		if(!expect_cur(p_state, kTok_Newline)) {
			return nullptr;
		}

		ast_label* label = new ast_label(get_token_string(id_tok));
		return label;
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
	p_state.sections = &sections;
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
			token directive_tok = p_state.current_token;
			cc::string directive_string = get_token_string(directive_tok);
			advance(p_state);
			if(directive_string == ".section") {
				if(!expect_cur(p_state, kTok_Directive)) {
					stop_parsing = true;
					break;
				}
								
				ast_section* sec = new ast_section(get_token_string(p_state.current_token));
				sections.push_back(sec);
			} else if(directive_string == ".global") {
				if(!expect_cur(p_state, kTok_Identifier)) {
					stop_parsing = true;
					break;
				}
				cc::string symbol_name = get_token_string(p_state.current_token);
				m_global_symbols.push_back(symbol_name);
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

cc::x86::instruction ast_instruction::gen_x86(cc::size_t instruction_offset) {
	using namespace cc::x86;
	
	// Handle "special" cases. << REALLY HACKY, FIX >>
	if(ins_name == "call") {
		if(first_operand != nullptr && first_operand->type == kAst_Label) {
			ast_label* label = (ast_label*)first_operand;
			
			// #todo(bwilks) -> fix, don't hardcode this
			const cc::size_t kSizeofCallInstruction = 5;
			const cc::i32 rel_offset = label->offset - (instruction_offset + kSizeofCallInstruction);
			instruction callIns = instruction::make_imm32_op(kCall, rel_offset);
			return callIns;
		} else {
			error_at_loc(loc, "Invalid call instruction.");
			return instruction::make_op(kNop);
		}
	}

	for(const auto& opcode_def : instructions_map) {
		instruction_def opcode = opcode_def.second;
		if(std::strcmp(opcode.get_name(), ins_name.c_str()) == 0) {
			instruction_operands_def mode;
			if(first_operand == nullptr && second_operand == nullptr) {
				mode = kInsOps_none;
			} else {
				if(second_operand == nullptr) {
					error_at_loc(loc, cc::format_string("Invalid instruction operands for '{0}'", ins_name));

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

	error_at_loc(loc, cc::format_string("No such instruction '{0}'", ins_name));
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
				
				int index = 0;
				for(ast_node* node : section->nodes) {
					switch(node->type) {
					case kAst_Instruction:
					{
						const cc::size_t ins_offset = instructions.offset_of_instruction(instructions.get_count());
						ast_instruction* instruction = (ast_instruction*)node;
						instructions.add(instruction->gen_x86(ins_offset));
						
						if(index > 0) {
							ast_node* prev = section->nodes[index - 1];
							if(prev->type == kAst_Label) {
								const cc::size_t offset = instructions.offset_of_instruction(instructions.get_count() - 1);
								ast_label* l = (ast_label*)prev;
								l->offset = offset;
							}
						}
						break;
					}
					case kAst_Label:
					{
						break;
					}
					default:
						CWARN("Cannot generate code for unknown node type");
						break;
					}
					index++;
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
	
	int section_index=1;
	for(ast_section* section : sections) {
		for(ast_node* node : section->nodes) {
			if(node->type == kAst_Label) {
				ast_label* label = (ast_label*) node;
				auto it = std::find(m_global_symbols.begin(), m_global_symbols.end(), label->name);
				
				if(it != m_global_symbols.end()) {
					cc::coff::symbol sym;
					sym.is_aux = false;
					sym.number_of_aux_symbols = 0;
					sym.name = cc::coff::symbol_name(label->name);
					sym.section_number = section_index;
					sym.storage_clss = cc::coff::kImageSymClassExternal;
					sym.value = label->offset;
					sym_table->add_symbol(sym);
				}
			}
		}
		section_index++;
	}

	object->set_symbol_table(sym_table);
	m_object_file = std::move(object);

	compiled_successfully = !s_found_errors;
}

