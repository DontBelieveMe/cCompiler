#include <cc/stdlib.h>
#include <cc/coff/object_file.h>
#include <cc/asm/asm_parser.h>
#include <cc/x86/instruction.h>

namespace cc {
	namespace assembly {
		class asm_file {
		public:
			asm_file(const cc::string& filepath);
			
			coff::object_file* get_obj_file() { return m_object_file.get(); }

			bool compiled_successfully = true;
		private:
			cc::unique_ptr<coff::object_file> m_object_file;	
			cc::array<cc::string> m_global_symbols;	
		private:
			cc::array<ast_section*> gen_ast(asm_parser& parser);
		};
	}
}
