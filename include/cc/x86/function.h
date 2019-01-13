#include <cc/stdlib.h>

#include <cc/x86/instruction.h>

namespace cc {
	namespace x86 {
		enum function_attrib {
			kFuncAttrib_External = 0,
			kFuncAttrib_Static = 1
		};

		class function {
		public:
			static function make_function(const cc::string& name, cc::u32 func_attribs);
			
			void add_instruction(const instruction& ins);
			void add_instructions(const cc::array<instruction>& instructions);

			instructions_collection& get_instructions()
				{ return m_instructions; }
		private:
			cc::string m_name;
			cc::x86::instructions_collection m_instructions;
			cc::u32 m_attribs;
		};
	}
}
