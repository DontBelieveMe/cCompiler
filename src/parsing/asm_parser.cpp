#include <cc/parsing/asm_parser.h>

#include <cc/file_io.h>
#include <cc/logging.h>
#include <cc/assert.h>
#include <cc/timing.h>

#include <regex>
#include <cctype>
#include <unordered_map>

#include <cc/x86/gp_register.h>
#include <cc/x86/instruction.h>

using namespace cc::parsing;

static std::unordered_map<cc::string, const cc::x86::gp_register*> s_reg_map = {
	{ "eax", &cc::x86::gp_register::eax },
	{ "ebx", &cc::x86::gp_register::ebx },
	{ "ecx", &cc::x86::gp_register::ecx },
	{ "edx", &cc::x86::gp_register::edx }
	
	// #todo (bwilks) -> fill the reset of these in
};

struct parse_instruction {
	union {
		cc::x86::gp_register reg1;
		cc::u32 imm1;
	};

	union {
		cc::x86::gp_register reg2;
		cc::u32 imm2;
	};

	cc::string op;
	cc::x86::instruction_operands_def operands_def; 
	
	parse_instruction() {}
	cc::x86::instruction gen_x86() {
		using namespace cc::x86;
		for(auto pair : instructions_map) {
			instruction_def opcode = pair.second;
			if(std::strcmp(opcode.get_name(), op.c_str()) == 0){
				if(opcode.get_operands_def() == operands_def) {
					switch(operands_def) {
					case kInsOps_rm32imm32:
						return instruction::make_reg_imm_op(pair.first, reg1, imm2);
					case kInsOps_r32rm32:
						return instruction::make_2reg_op(pair.first, reg1, reg2);
					case kInsOps_none:
						return instruction::make_op(pair.first);
					}
				}
			}
		}
		
		CWARN("Cannot generate x86 instruction \"{0}\"", op); 
		return instruction::make_op(kNop);
	}
};

asm_file asm_file::from_file(const cc::string& filepath) {
	asm_file assembly_file;
	
	cc::file file(filepath);
	file.read(cc::kFileModeText);

	cc::array<cc::u8>& file_dat = file.data_array();
	cc::string file_text(file_dat.begin(), file_dat.end());
	
	cc::size_t current_line = 1;
	cc::size_t current_col = 1;
	cc::size_t current_index = 0;
	
	cc::array<parse_instruction> instructions;
	
	cc::time_block scope_perf([&](float seconds){
		CINFO("Parsed {0} lines in {1}s", current_line, seconds);
	});

	while(current_index < file_text.length()) {
		while(file_text[current_index] == ' ' || file_text[current_index] == '\t') {
			current_index++;
			current_col++;
		}

		char cchar = file_text[current_index];
		
		// parsing section identifier
		if(cchar == '.') {
			cc::string section_name;
			current_index++;
			cchar = file_text[current_index];
			
			while(isalpha(cchar)) {
				section_name += cchar;
				current_index++;
				cchar = file_text[current_index];
			}

			current_col += section_name.length();
		}
		
		if(cchar == ';') {
			while(cchar != '\n') {
				current_index++;
				cchar = file_text[current_index];
			}
		}

		// parsing instruction
		if(isalpha(cchar)) {
			cc::string ins_name;
			parse_instruction ins;

			const char* start_ptr = &file_text[current_index];
			
			while(isalpha(cchar)) {
				ins_name += cchar;
				current_index++;
				cchar = file_text[current_index];
			}

			ins.operands_def = cc::x86::kInsOps_none;	
			
			if(cchar == ' ') {
				while(cchar == ' ' ||  cchar == '\t') cchar = file_text[++current_index];
				
				if(isalpha(cchar)) {
					cc::string op1_name;
					
					while(isalpha(cchar)) {
						op1_name += cchar;
						cchar = file_text[++current_index];
					}
			
					ins.reg1 = *s_reg_map[op1_name];
				}
				
				ASSERT(!isdigit(cchar), "First operand cannot be a numeric literal!");
				
				while(cchar == ' ' || cchar == '\t')
					cchar = file_text[++current_index];
				
				if(cchar == ',') {
					cchar = file_text[++current_index];
					
					while(cchar == ' ' || cchar == '\t')
						cchar = file_text[++current_index];
	
					cc::string op2;
					while(cchar != '\n') {
						op2 += cchar;
						cchar = file_text[++current_index];
					}

					if(isdigit(op2[0])) {
						ins.imm2 = std::stoi(op2);
						ins.operands_def = cc::x86::kInsOps_rm32imm32;
					} else  {
						ins.reg2 = *s_reg_map[op2];
						ins.operands_def = cc::x86::kInsOps_r32rm32;
					}
				}
			}

			while(cchar != '\n') {
				current_index++;
				cchar = file_text[current_index];
			}

			ins.op = ins_name;
			instructions.push_back(ins);
			
			const char* end_ptr = &file_text[current_index];
			current_col += end_ptr  - start_ptr;
		}

		if(cchar == '\n') {
			current_line++;
			current_col = 1;
		} else {
			current_col++;
		}

		current_index++;
	}

	for(parse_instruction& ins : instructions) {
		assembly_file.m_instructions.add(ins.gen_x86());
	}

	return assembly_file;
}
