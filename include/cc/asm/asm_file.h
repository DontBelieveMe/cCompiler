#include <cc/stdlib.h>
#include <cc/coff/object_file.h>

namespace cc {
	namespace assembly {
		class asm_file {
		public:
			asm_file(const cc::string& filepath);
		private:
			cc::unique_ptr<coff::object_file> m_object_file;	
		};
	}
}
