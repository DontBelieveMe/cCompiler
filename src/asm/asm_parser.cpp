#include <cc/asm/asm_parser.h>

#include <cc/file_io.h>
#include <cc/logging.h>
#include <cc/assert.h>
#include <cc/timing.h>

#include <regex>
#include <cctype>
#include <unordered_map>

#include <cc/x86/gp_register.h>
#include <cc/x86/instruction.h>

using namespace cc::assembly;

Token::Token(ETokenType type, char* data, cc::size_t data_length, cc::size_t line, cc::size_t col, cc::size_t idx)
	: m_type(type), m_data(data), m_data_length(data_length), loc(FileLocation(line, col, idx)) {
}

// These need to be pointers to the registers because, this static array
// is not necessarily initialized after the registers have been (which are
// also static data). They are pointers so that when they need to be looked
// up they reference the original.
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

struct parse_instruction {
	union {
		cc::x86::GeneralPurposeRegister reg1;
		cc::u32 imm1;
	};

	union {
		cc::x86::GeneralPurposeRegister reg2;
		cc::u32 imm2;
	};

	cc::String op;
	cc::x86::EInstructionOperandsDef operands_def; 
	
	parse_instruction() {}

	cc::x86::Instruction gen_x86(); 
};

static bool is_whitespace(char c) {
	return c == ' ' || c == '\t';
}

AsmParser::AsmParser(cc::String&& text) 
	: m_cline(1), m_ccol(1), m_cindex(0), m_text(text) {
}

char AsmParser::next_non_whitespace_char() {
	char cur_char = current_char();

	while(is_whitespace(cur_char)) {
		cur_char = advance_char();
	}

	return cur_char;
}

void cc::assembly::emit_parse_error(cc::size_t line, cc::size_t col, cc::size_t gidx, const cc::String& file_text, const cc::String& msg) {
	// Let's do some cool visualizations :D
	// #todo (bwilks) @FixMe @FixMe @FixMe
	// this code is really messy & dirty, but it does the job for now
	// -=- clean up -=-
	
	// Print out specific error message - e.g. "Unknown character...";
	// As well as the line and column numbers.
	CFATAL("({0}, {1}): error: {2}", line, col, msg);
	
	const cc::size_t kCol = col;
	cc::size_t idx = gidx - (kCol - 1);
	
	// Print out the line on which the error occurs.
	cc::String line1;
	char c = file_text[idx];
	cc::size_t line_len = 0;
	while(c != '\n') {
		line1 += c;
		c = file_text[++idx];
		line_len++;
	}
	CFATAL("\t{0}", line1);
	
	// Print out a '^' under the character where the error has occurred
	cc::size_t col_tmp = 1;
	cc::String line2 = "";
	while(col_tmp <= line_len) {
		if(col_tmp == kCol) {
			line2 += '^';
		} else {
			if(line1[col_tmp - 1] == '\t')
				line2 += '\t';
			else
				line2 += ' ';
		}
		col_tmp++;
	}
	CFATAL("\t{0}", line2);
	
}

Token AsmParser::parse_error_here(const cc::String& msg) {
	emit_parse_error(m_cline, m_ccol, m_cindex, m_text, msg);
	return Token(kTok_EOF, nullptr, 0, m_cline, m_ccol, m_cindex);
}

Token AsmParser::parse_next_token() {
	if(current_char() == '\n') {
		char* start = &m_text[m_cindex];
		advance_char();
		return Token(kTok_Newline, start, 1, m_cline, m_ccol, m_cindex);
	}

	char cur_char = next_non_whitespace_char();

	while(cur_char == ';') {
		while(cur_char != '\n') {
			cur_char = advance_char();
		}
		cur_char = advance_char();
	}
	
	if(current_char() == '\n') {
		char* start = &m_text[m_cindex];
		advance_char();
		return Token(kTok_Newline, start, 1, m_cline, m_ccol, m_cindex);
	}

	cur_char = next_non_whitespace_char();
	
	Token tok;
	
	const cc::size_t iline = m_cline, icol = m_ccol, iidx = m_cindex;

	// Parse directive
	if(cur_char == '.') {
		tok.set_type(kTok_Directive);
		
		char* start = &m_text[m_cindex];
		cc::size_t len = 0;
		
		while(cur_char == '.' || isalpha(cur_char)) {
			len++;
			cur_char = advance_char();
		}
		
		tok.set_data(start);
		tok.set_data_length(len);
		
		tok.set_location(FileLocation(iline, icol, iidx));

		return tok;
	}

	if(isalpha(cur_char) || cur_char == '_') {
		char* start = &m_text[m_cindex];
		cc::size_t len = 1;
		cur_char = advance_char();
		while(isalnum(cur_char) || cur_char == '_') {
			len++;
			cur_char = advance_char();
		}

		return Token(kTok_Identifier, start, len, iline, icol, iidx);
	}

	if(cur_char == ',') {
		char* start = &m_text[m_cindex];
		cur_char = advance_char();
		return Token(kTok_Comma, start, 1, iline, icol, iidx);
	}
	
	if(cur_char == ':') {
		char *start = &m_text[m_cindex];
		cur_char = advance_char();
		return Token(kTok_Colon, start, 1, iline, icol, iidx);
	}

	if(isdigit(cur_char)) {
		char* start = &m_text[m_cindex];
		cc::size_t len = 0;
		while(isdigit(cur_char)) {
			len++;
			cur_char = advance_char();
		}

		return Token(kTok_IntLiteral, start, len, iline, icol, iidx);
	}
		
	if(cur_char != '\0') {
		parse_error_here(cc::format_string("Unknown character '{0}'", cur_char));
		
		while (cur_char != '\n') {
			cur_char = advance_char();
		}

		return Token(kTok_Error, &m_text[m_cindex], 1, m_cline, m_ccol, m_cindex);
	}

	return Token(kTok_EOF, "0", 1, iline, icol, iidx);
}

char AsmParser::current_char() {
	return m_text[m_cindex];
}

char AsmParser::advance_char() {
	char cur_char = current_char();
	
	if(cur_char == '\n') {
		m_cline++;
		m_ccol = 1;
	} else {
		m_ccol++;
	}

	m_cindex++;
	return current_char();
}

AsmParser AsmParser::from_file(const cc::String& filepath) {
	cc::File file(filepath);
	file.read(cc::kFileModeText);

	cc::Array<cc::u8>& file_dat = file.data_array();
	cc::String file_text(file_dat.begin(), file_dat.end());
		
	AsmParser assembly_file(std::move(file_text));
	
	return assembly_file;
}

cc::x86::Instruction parse_instruction::gen_x86() {
	using namespace cc::x86;
	for(const auto& opcode_def_pair  : instructions_map) {
		InstructionDef opcode = opcode_def_pair.second;
		if(std::strcmp(opcode.get_name(), op.c_str()) == 0){
			if(opcode.get_operands_def() == operands_def) {
				switch(operands_def) {
				case kInsOps_rm32imm32:
					return Instruction::make_reg_imm_op(opcode_def_pair.first, reg1, imm2);
				case kInsOps_r32rm32:
					return Instruction::make_2reg_op(opcode_def_pair.first, reg1, reg2);
				case kInsOps_none:
					return Instruction::make_op(opcode_def_pair.first);
				}
			}
		}
	}
	
	CWARN("Cannot generate x86 instruction \"{0}\"", op); 
	return Instruction::make_op(kNop);
}
