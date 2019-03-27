#pragma once

#include <cc/stdlib.h>
#include <cc/x86/instruction.h>
#include <cc/logging.h>

#include <unordered_map>

namespace cc {
	namespace assembly {
		struct FileLocation {
			cc::size_t line;
			cc::size_t col;
			cc::size_t index;

			FileLocation(cc::size_t line, cc::size_t col, cc::size_t index) 
				: line(line), col(col), index(index) {
			}
			
			FileLocation(){}
		};

		enum EAstNodeType {
			kAst_Instruction, kAst_Directive, kAst_Unknown, kAst_IntLiteral, kAst_Label, kAst_Register
		};

		struct AstNode {
			EAstNodeType type = kAst_Unknown;
			//token tok;
			FileLocation loc;
		};

		struct AstRegister : public AstNode {
			AstRegister(cc::String reg_name);
			const cc::x86::GeneralPurposeRegister* reg;
		};

		struct AstIntLiteral : public AstNode {
			AstIntLiteral(cc::u32 number) {
				type = kAst_IntLiteral;
				this->number = number;
			}

			cc::u32 number;
		};

		struct AstLabel : public AstNode {
			AstLabel(cc::String label_name) {
				type = kAst_Label;
				name = label_name;
			}

			cc::String name;
			cc::size_t offset = 0;
		};

		struct AstInstruction : public AstNode {
			AstInstruction(cc::String op, AstNode* operand1, AstNode* operand2) {
				type = kAst_Instruction;

				first_operand = operand1;
				second_operand = operand2;
				ins_name = op;
			}
			
			cc::x86::Instruction gen_x86(cc::size_t offset);	

			cc::String ins_name;
			AstNode* first_operand;
			AstNode* second_operand;
		};

		struct AstDirective : public AstNode {
			AstDirective() {
				type = kAst_Directive;
			}
		};

		struct AstSection {	
			AstSection(cc::String name) {
				this->name = name;
			}

			cc::String name;
			cc::Array<AstNode*> nodes;
		};

		enum ETokenType {
			kTok_Directive,
			kTok_Identifier,
			kTok_IntLiteral,
			kTok_Comma,
			kTok_EOF,
			kTok_Newline,
			kTok_Colon,
			kTok_Error
		};

		class Token {
		public:
			Token(ETokenType type, char *data, cc::size_t data_length, cc::size_t line, cc::size_t col, cc::size_t idx);
			Token() {}
			
			void set_type(ETokenType t) {
				m_type = t;
			}

			void set_data(char* dat) {
				m_data = dat;
			}

			void set_data_length(cc::size_t len) {
				m_data_length = len;
			}

			ETokenType get_type() const {
				return m_type;
			}

			char* get_data() const {
				return m_data;
			}

			cc::size_t get_data_length() const {
				return m_data_length;
			}

			FileLocation get_location() const {
				return loc;
			}

			void set_location(FileLocation loc) {
				this->loc = loc;
			}

		private:
			ETokenType m_type;
			char* m_data;
			cc::size_t m_data_length;
			FileLocation loc;
		};

		class AsmParser {
		public:
			AsmParser(cc::String&& text);

			static AsmParser from_file(const cc::String& filepath);
			cc::x86::InstructionsCollection& get_instructions() { return m_instructions; }
			Token parse_next_token();
			cc::String& get_text() { return m_text; }
			
			cc::size_t get_lines_parsed() { return m_cline - 1; }
			
		private:
			cc::x86::InstructionsCollection m_instructions;
			
			cc::size_t m_cline, m_ccol, m_cindex;
			cc::String m_text;

		private:
			char advance_char();
			char current_char();

			char next_non_whitespace_char();
			
			Token parse_error_here(const cc::String& msg);
		};

		void emit_parse_error(cc::size_t line, cc::size_t col, cc::size_t gidx, const cc::String& file_text, const cc::String& msg);	
	}
}
