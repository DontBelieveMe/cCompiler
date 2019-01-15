#pragma once

#include <cc/stdlib.h>
#include <cc/x86/instruction.h>
#include <cc/logging.h>

#include <unordered_map>

namespace cc {
	namespace assembly {
		enum token_type {
			kTok_Directive,
			kTok_Identifier,
			kTok_IntLiteral,
			kTok_Comma,
			kTok_EOF,
			kTok_Newline,
			kTok_Colon
		};

		class token {
		public:
			token(token_type type, char *data, cc::size_t data_length, cc::size_t line, cc::size_t col, cc::size_t idx);
			token() {}
			
			void set_type(token_type t) {
				m_type = t;
			}

			void set_index(cc::size_t idx) {
				m_index = idx;
			}

			void set_line_col(cc::size_t line, cc::size_t col) {
				m_line = line;
				m_col = col;
			}

			void set_data(char* dat) {
				m_data = dat;
			}

			void set_data_length(cc::size_t len) {
				m_data_length = len;
			}

			cc::size_t get_line() const {
				return m_line;
			}
			
			cc::size_t get_col() const {
				return m_col;
			}

			cc::size_t get_index() const {
				return m_index;
			}

			token_type get_type() const {
				return m_type;
			}

			char* get_data() const {
				return m_data;
			}

			cc::size_t get_data_length() const {
				return m_data_length;
			}

		private:
			token_type m_type;
			char* m_data;
			cc::size_t m_data_length;
			cc::size_t m_line, m_col, m_index;
		};

		class asm_parser {
		public:
			asm_parser(cc::string&& text);

			static asm_parser from_file(const cc::string& filepath);
			cc::x86::instructions_collection& get_instructions() { return m_instructions; }
			token parse_next_token();
			cc::string& get_text() { return m_text; }
			
			cc::size_t get_lines_parsed() { return m_cline; }

		private:
			cc::x86::instructions_collection m_instructions;
			
			cc::size_t m_cline, m_ccol, m_cindex;
			cc::string m_text;

		private:
			char advance_char();
			char current_char();

			char next_non_whitespace_char();
			
			token parse_error_here(const cc::string& msg);
		};

		void emit_parse_error(cc::size_t line, cc::size_t col, cc::size_t gidx, const cc::string& file_text, const cc::string& msg);	
	}
}
