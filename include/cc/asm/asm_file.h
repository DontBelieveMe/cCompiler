#include <cc/stdlib.h>
#include <cc/coff/object_file.h>
#include <cc/asm/asm_parser.h>
#include <cc/x86/instruction.h>

namespace cc {
	namespace assembly {
		class AsmFile {
		public:
			AsmFile(const cc::String& filepath);
			
			coff::ObjectFile* get_obj_file() { return m_object_file.get(); }

			bool compiled_successfully = true;
		private:
			cc::UniquePtr<coff::ObjectFile> m_object_file;	
			cc::Array<cc::String> m_global_symbols;	
		private:
			cc::Array<AstSection*> gen_ast(AsmParser& parser);
		};
	}
}
