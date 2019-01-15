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

token::token(token_type type, char* data, cc::size_t data_length, cc::size_t line, cc::size_t col, cc::size_t idx)
	: m_type(type), m_data(data), m_data_length(data_length), m_line(line), m_col(col), m_index(idx) {
}

// These need to be pointers to the registers because, this static array
// is not nessersarily initalized after the registers have been (which are
// also static data). They are pointers so that when they need to be looked
// up they reference the original.
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

	cc::x86::instruction gen_x86(); 
};

static bool is_whitespace(char c) {
	return c == ' ' || c == '\t';
}

asm_parser::asm_parser(cc::string&& text) 
	: m_cline(1), m_ccol(1), m_cindex(0), m_text(text) {
}

char asm_parser::next_non_whitespace_char() {
	char cur_char = current_char();

	while(is_whitespace(cur_char)) {
		cur_char = advance_char();
	}

	return cur_char;
}

void cc::assembly::emit_parse_error(cc::size_t line, cc::size_t col, cc::size_t gidx, const cc::string& file_text, const cc::string& msg) {
	// Let's do some cool visualisations :D
	// #todo (bwilks) @FixMe @FixMe @FixMe
	// this code is really messy & dirty, but it does the job for now
	// -=- clean up -=-
	
	// Print out specific error message - e.g. "Unkown character...";
	// As well as the line and column numbers.
	CFATAL("({0}, {1}): error: {2}", line, col, msg);
	
	const cc::size_t kCol = col;
	cc::size_t idx = gidx - (kCol - 1);
	
	// Print out the line on which the error occurs.
	cc::string line1;
	char c = file_text[idx];
	cc::size_t line_len = 0;
	while(c != '\n') {
		line1 += c;
		c = file_text[++idx];
		line_len++;
	}
	CFATAL("\t{0}", line1);
	
	// Print out a '^' under the character where the error has occured
	cc::size_t col_tmp = 1;
	cc::string line2 = "";
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

token asm_parser::parse_error_here(const cc::string& msg) {
	emit_parse_error(m_cline, m_ccol, m_cindex, m_text, msg);
	return token(kTok_EOF, nullptr, 0, m_cline, m_ccol, m_cindex);
}

token asm_parser::parse_next_token() {
	if(current_char() == '\n') {
		char* start = &m_text[m_cindex];
		advance_char();
		return token(kTok_Newline, start, 1, m_cline, m_ccol, m_cindex);
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
		return token(kTok_Newline, start, 1, m_cline, m_ccol, m_cindex);
	}

	cur_char = next_non_whitespace_char();
	
	token tok;
	
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
		tok.set_line_col(iline, icol);
		tok.set_index(iidx);

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

		return token(kTok_Identifier, start, len, iline, icol, iidx);
	}

	if(cur_char == ',') {
		char* start = &m_text[m_cindex];
		cur_char = advance_char();
		return token(kTok_Comma, start, 1, iline, icol, iidx);
	}
	
	if(cur_char == ':') {
		char *start = &m_text[m_cindex];
		cur_char = advance_char();
		return token(kTok_Colon, start, 1, iline, icol, iidx);
	}

	if(isdigit(cur_char)) {
		char* start = &m_text[m_cindex];
		cc::size_t len = 0;
		while(isdigit(cur_char)) {
			len++;
			cur_char = advance_char();
		}

		return token(kTok_IntLiteral, start, len, iline, icol, iidx);
	}
		
	if(cur_char != '\0') {
		parse_error_here(cc::format_string("Unknown character '{0}'", cur_char));	
	}

	return token(kTok_EOF, &m_text[0], 1, iline, icol, iidx);
}

char asm_parser::current_char() {
	return m_text[m_cindex];
}

char asm_parser::advance_char() {
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

asm_parser asm_parser::from_file(const cc::string& filepath) {
	cc::file file(filepath);
	file.read(cc::kFileModeText);

	cc::array<cc::u8>& file_dat = file.data_array();
	cc::string file_text(file_dat.begin(), file_dat.end());
		
	asm_parser assembly_file(std::move(file_text));
	
	return assembly_file;
}

cc::x86::instruction parse_instruction::gen_x86() {
	using namespace cc::x86;
	for(const auto& opcode_def_pair  : instructions_map) {
		instruction_def opcode = opcode_def_pair.second;
		if(std::strcmp(opcode.get_name(), op.c_str()) == 0){
			if(opcode.get_operands_def() == operands_def) {
				switch(operands_def) {
				case kInsOps_rm32imm32:
					return instruction::make_reg_imm_op(opcode_def_pair.first, reg1, imm2);
				case kInsOps_r32rm32:
					return instruction::make_2reg_op(opcode_def_pair.first, reg1, reg2);
				case kInsOps_none:
					return instruction::make_op(opcode_def_pair.first);
				}
			}
		}
	}
	
	CWARN("Cannot generate x86 instruction \"{0}\"", op); 
	return instruction::make_op(kNop);
}
