#pragma once

#include <cc/stdlib.h>
#include <cc/x86/instruction.h>
#include <cc/logging.h>

#include <unordered_map>

namespace cc {
	namespace assembly {
		struct file_location {
			cc::size_t line;
			cc::size_t col;
			cc::size_t index;

			file_location(cc::size_t line, cc::size_t col, cc::size_t index) 
				: line(line), col(col), index(index) {
			}
			
			file_location(){}
		};

		enum ast_node_type {
			kAst_Instruction, kAst_Directive, kAst_Unknown, kAst_IntLiteral, kAst_Label, kAst_Register
		};

		struct ast_node {
			ast_node_type type = kAst_Unknown;
			//token tok;
			file_location loc;
		};

		struct ast_register : public ast_node {
			ast_register(cc::string reg_name);
			const cc::x86::gp_register* reg;
		};

		struct ast_int_literal : public ast_node {
			ast_int_literal(cc::u32 number) {
				type = kAst_IntLiteral;
				this->number = number;
			}

			cc::u32 number;
		};

		struct ast_label : public ast_node {
			ast_label(cc::string label_name) {
				type = kAst_Label;
				name = label_name;
			}

			cc::string name;
			cc::size_t offset = 0;
		};

		struct ast_instruction : public ast_node {
			ast_instruction(cc::string op, ast_node* operand1, ast_node* operand2) {
				type = kAst_Instruction;

				first_operand = operand1;
				second_operand = operand2;
				ins_name = op;
			}
			
			cc::x86::instruction gen_x86(cc::size_t offset);	

			cc::string ins_name;
			ast_node* first_operand;
			ast_node* second_operand;
		};

		struct ast_directive : public ast_node {
			ast_directive() {
				type = kAst_Directive;
			}
		};

		struct ast_section {	
			ast_section(cc::string name) {
				this->name = name;
			}

			cc::string name;
			cc::array<ast_node*> nodes;
		};

		enum token_type {
			kTok_Directive,
			kTok_Identifier,
			kTok_IntLiteral,
			kTok_Comma,
			kTok_EOF,
			kTok_Newline,
			kTok_Colon,
			kTok_Error
		};

		class token {
		public:
			token(token_type type, char *data, cc::size_t data_length, cc::size_t line, cc::size_t col, cc::size_t idx);
			token() {}
			
			void set_type(token_type t) {
				m_type = t;
			}

			void set_data(char* dat) {
				m_data = dat;
			}

			void set_data_length(cc::size_t len) {
				m_data_length = len;
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

			file_location get_location() const {
				return loc;
			}

			void set_location(file_location loc) {
				this->loc = loc;
			}

		private:
			token_type m_type;
			char* m_data;
			cc::size_t m_data_length;
			file_location loc;
		};

		class asm_parser {
		public:
			asm_parser(cc::string&& text);

			static asm_parser from_file(const cc::string& filepath);
			cc::x86::instructions_collection& get_instructions() { return m_instructions; }
			token parse_next_token();
			cc::string& get_text() { return m_text; }
			
			cc::size_t get_lines_parsed() { return m_cline - 1; }
			
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
