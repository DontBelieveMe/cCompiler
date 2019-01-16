#include <cc/stdlib.h>
#include <cc/coff/object_file.h>
#include <cc/asm/asm_parser.h>
#include <cc/x86/instruction.h>

namespace cc {
	namespace assembly {
		enum ast_node_type {
			kAst_Instruction, kAst_Directive, kAst_Unknown, kAst_IntLiteral, kAst_Label, kAst_Register
		};

		struct ast_node {
			ast_node_type type = kAst_Unknown;
			token tok;
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

		class asm_file {
		public:
			asm_file(const cc::string& filepath);
			
			coff::object_file* get_obj_file() { return m_object_file.get(); }
		private:
			cc::unique_ptr<coff::object_file> m_object_file;	
			cc::array<cc::string> m_global_symbols;	
		private:
			cc::array<ast_section*> gen_ast(asm_parser& parser);
		};
	}
}
