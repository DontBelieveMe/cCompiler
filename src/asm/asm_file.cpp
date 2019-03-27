#include <cc/asm/asm_file.h>

#include <cc/x86/instruction.h>
#include <cc/x86/gp_register.h>
#include <cc/coff/section.h>
#include <cc/timing.h>
#include <cc/assert.h>
#include <algorithm>

using namespace cc::assembly;

struct parser_state {
	AsmParser* parser;
	Token current_token;
	cc::Array<AstSection*> *sections;
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
static cc::String *s_file_text = nullptr;
static bool s_found_errors = false;

static void error_here(parser_state& parser, const cc::String& msg) {
	Token& cur_tok = parser.current_token;
	FileLocation loc = cur_tok.get_location();
	s_found_errors = true;
	emit_parse_error(loc.line, loc.col, loc.index, parser.parser->get_text(), msg);
}

static void error_at_loc(FileLocation loc, const cc::String& msg) {
	emit_parse_error(loc.line, loc.col, loc.index, *s_file_text, msg);	
	s_found_errors = true;
}

static void advance(parser_state& parser) {
	parser.current_token = parser.parser->parse_next_token();
}

static cc::String get_token_string(const Token& tok) {
	cc::String tok_str(tok.get_data(), tok.get_data() + tok.get_data_length());
	return tok_str;
}

// Returns true if the current token is of the correct type
// false & error message otherwise.
static bool expect_cur(parser_state& parser, ETokenType expected_type) {
	if (parser.current_token.get_type() == kTok_Error) {
		return true;
	}

	if(parser.current_token.get_type() != expected_type) {
		cc::String tok_str = get_token_string(parser.current_token);
		error_here(parser, cc::format_string("Unexpected token '{0}'.", tok_str));
		return false;
	}	
	return true;
}

static bool accept(parser_state& parser, ETokenType expected) {
	if(parser.current_token.get_type() == expected || parser.current_token.get_type() == kTok_Error) {
		advance(parser);
		return true;
	}

	return false;
}

static std::unordered_map<cc::String, const cc::x86::GeneralPurposeRegister*> s_reg_map = {
	{ "eax", &cc::x86::GeneralPurposeRegister::eax },
	{ "ebx", &cc::x86::GeneralPurposeRegister::ebx },
	{ "ecx", &cc::x86::GeneralPurposeRegister::ecx },
	{ "edx", &cc::x86::GeneralPurposeRegister::edx },
	{ "esp", &cc::x86::GeneralPurposeRegister::esp },
	{ "ebp", &cc::x86::GeneralPurposeRegister::ebp },
	{ "esi", &cc::x86::GeneralPurposeRegister::esi },
	{ "edi", &cc::x86::GeneralPurposeRegister::edi }
};

AstRegister::AstRegister(cc::String reg_name) {
	type = kAst_Register;
	reg = s_reg_map[reg_name];
}

static bool is_register(const cc::String& s) {
	return s_reg_map.find(s) != s_reg_map.end();
}

static AstNode* parse_instruction(parser_state& p_state) {
	const Token id_tok = p_state.current_token;
	
	advance(p_state);
	
	AstInstruction* ins = new AstInstruction(get_token_string(id_tok), nullptr, nullptr);
	ins->loc = FileLocation(id_tok.get_location());	
	
	// Will either be the first operand or a newline (in the event the instruction has no operands)
	const Token operand1_tok = p_state.current_token;
	const cc::String operand1_string = get_token_string(operand1_tok);
	
	// The first operand to this instruction is a identifier (not a numeric literal)
	if(accept(p_state, kTok_Identifier)) {
		if(!is_register(operand1_string)) {
			// #todo (bwilks) -> this is only until labels are implemented (to refer to memory locations)
			// need to look into whether instructions even allow reg/memory first operands anyway 
			//error_at_tok(operand1_tok, cc::format_string("Register '{0}' does not exist.", operand1_string));
			//ast_label* label = new ast_label(operand1_string);
			for (AstSection* section : *p_state.sections) {
				for (AstNode* node : section->nodes) {
					if (node->type == kAst_Label) {
						AstLabel* l = (AstLabel*)node;
						if (l->name == operand1_string) {
							ins->first_operand = l;
						}
					}
				}
			}
			if(!ins->first_operand) {
				error_at_loc(operand1_tok.get_location(), cc::format_string("Unknown label {0}", operand1_string));
				delete ins;
				return nullptr;
			}
		} else {
			ins->first_operand = new AstRegister(operand1_string);
		}

		if(accept(p_state, kTok_Comma)) {
			// Cache the second operands token, as the following `advance` calls
			// will change the parse states current token so that it no longer represents 
			// the second operand
			const Token operand2_tok = p_state.current_token;
			const cc::String operand2_string = get_token_string(operand2_tok);

			if(accept(p_state, kTok_IntLiteral)) {
				ins->second_operand = new AstIntLiteral(std::stoi(operand2_string));
			} else if(accept(p_state, kTok_Identifier)) {
				// See if the second operand is a register or a label
				const bool second_operand_is_register = is_register(operand2_string);
				
				if(second_operand_is_register) {
					ins->second_operand = new AstRegister(operand2_string);
				} else {
					// #todo (bwilks) -> fix (remove when labels & memory addressing is implemented)
					// see previous todo
					//error_at_tok(operand2_tok, "Label (memory address) operands are not currently supported!");
					error_at_loc(operand2_tok.get_location(), "Labels as secondary operands are not currently supported!");
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
		ins->first_operand = new AstIntLiteral(std::stoi(operand1_string));
	} else if(accept(p_state, kTok_Colon)) {
		// It's a label!
		
		delete ins;
		
		if(!expect_cur(p_state, kTok_Newline)) {
			return nullptr;
		}

		AstLabel* label = new AstLabel(get_token_string(id_tok));
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

cc::Array<AstSection*> AsmFile::gen_ast(AsmParser& parser) {
	TimeBlock perf([&](float s){
		CINFO("Parsed {0} lines in {1}s", parser.get_lines_parsed(), s);
	});
	
	parser_state p_state;
	p_state.parser = &parser;
	p_state.current_token = parser.parse_next_token();
	
	s_file_text = &parser.get_text();

	cc::Array<AstSection*> sections;
	bool stop_parsing = false;
	p_state.sections = &sections;
	for(;;) {
		Token& ctok = p_state.current_token;
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
			AstNode* node = parse_instruction(p_state);
			
			// there has been a parse error occur while parsing the instruction
			// so stop parsing
			if(!node) {
				stop_parsing = true;
				break;
			}

			AstSection* current_section = sections.back();
			current_section->nodes.push_back(node);

			break;
		}	
		case kTok_Directive:
		{
			Token directive_tok = p_state.current_token;
			cc::String directive_string = get_token_string(directive_tok);
			advance(p_state);
			if(directive_string == ".section") {
				if(!expect_cur(p_state, kTok_Directive)) {
					stop_parsing = true;
					break;
				}
								
				AstSection* sec = new AstSection(get_token_string(p_state.current_token));
				sections.push_back(sec);
			} else if(directive_string == ".global") {
				if(!expect_cur(p_state, kTok_Identifier)) {
					stop_parsing = true;
					break;
				}
				cc::String symbol_name = get_token_string(p_state.current_token);
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

cc::x86::Instruction AstInstruction::gen_x86(cc::size_t instruction_offset) {
	using namespace cc::x86;
	
	// Handle "special" cases. << REALLY HACKY, FIX >>
	if(ins_name == "call") {
		if(first_operand != nullptr && first_operand->type == kAst_Label) {
			AstLabel* label = (AstLabel*)first_operand;
			
			// #todo(bwilks) -> fix, don't hardcode this
			const cc::size_t kSizeofCallInstruction = 5;
			const cc::i32 rel_offset = label->offset - (instruction_offset + kSizeofCallInstruction);
			Instruction callIns = Instruction::make_imm32_op(kCall, rel_offset);
			return callIns;
		} else {
			error_at_loc(loc, "Invalid call instruction.");
			return Instruction::make_op(kNop);
		}
	}

	for(const auto& opcode_def : instructions_map) {
		InstructionDef opcode = opcode_def.second;
		if(std::strcmp(opcode.get_name(), ins_name.c_str()) == 0) {
			EInstructionOperandsDef mode;
			if(first_operand == nullptr && second_operand == nullptr) {
				mode = kInsOps_none;
			} else {
				if(second_operand == nullptr) {
					error_at_loc(loc, cc::format_string("Invalid instruction operands for '{0}'", ins_name));

					return Instruction::make_op(kNop);
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
					AstRegister* r = (AstRegister*)first_operand;
					AstIntLiteral* i = (AstIntLiteral*)second_operand;
					return Instruction::make_reg_imm_op(opcode_def.first, *(r->reg), i->number);
				}
				case kInsOps_r32rm32:
				{
					AstRegister* r = (AstRegister*)first_operand;
					AstRegister* r2 = (AstRegister*)second_operand;
					return Instruction::make_2reg_op(opcode_def.first, *(r->reg), *(r2->reg));
				}
				case kInsOps_none:
					return Instruction::make_op(opcode_def.first);
				}
			}
		}
	}

	error_at_loc(loc, cc::format_string("No such instruction '{0}'", ins_name));
	return Instruction::make_op(kNop);
}

AsmFile::AsmFile(const cc::String& filepath) {
	AsmParser parser = AsmParser::from_file(filepath);
	cc::Array<AstSection*> sections = gen_ast(parser);

	cc::UniquePtr<coff::ObjectFile> object = cc::make_unique<coff::ObjectFile>();
	{
		cc::TimeBlock perf([](float seconds){
			CINFO("Code generation took {0}s", seconds);
		});

		for(AstSection* section : sections) {
			cc::SharedPtr<cc::coff::Section> obj_section = cc::make_shared<cc::coff::Section>();
			obj_section->set_name(section->name);
			cc::u32 characteristics = 0;

			if(section->name == ".text") {
				characteristics = cc::coff::kImageScnMemExecute | cc::coff::kImageScnCntCode | cc::coff::kImageScnMemRead;
				obj_section->set_characteristics(characteristics);

				cc::x86::InstructionsCollection instructions;
				
				int index = 0;
				for(AstNode* node : section->nodes) {
					switch(node->type) {
					case kAst_Instruction:
					{
						const cc::size_t ins_offset = instructions.offset_of_instruction(instructions.get_count());
						AstInstruction* instruction = (AstInstruction*)node;
						instructions.add(instruction->gen_x86(ins_offset));
						
						if(index > 0) {
							AstNode* prev = section->nodes[index - 1];
							if(prev->type == kAst_Label) {
								const cc::size_t offset = instructions.offset_of_instruction(instructions.get_count() - 1);
								AstLabel* l = (AstLabel*)prev;
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
	
	cc::String drectve_str = "/ENTRY:main ";

	cc::SharedPtr<coff::Section> drectve = cc::make_shared<coff::Section>();
	drectve->set_name(".drectve")
		.set_characteristics(cc::coff::kImageScnLnkRemove | cc::coff::kImageScnLnkInfo)
		.set_raw_data(cc::Array<cc::u8>(drectve_str.begin(), drectve_str.end()));

	object->add_section(drectve);
	
	cc::SharedPtr<cc::coff::SymbolTable> sym_table = cc::make_shared<cc::coff::SymbolTable>();
	
	int section_index=1;
	for(AstSection* section : sections) {
		for(AstNode* node : section->nodes) {
			if(node->type == kAst_Label) {
				AstLabel* label = (AstLabel*) node;
				auto it = std::find(m_global_symbols.begin(), m_global_symbols.end(), label->name);
				
				if(it != m_global_symbols.end()) {
					cc::coff::Symbol sym;
					sym.is_aux = false;
					sym.number_of_aux_symbols = 0;
					sym.name = cc::coff::SymbolName(label->name);
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

