#include <cc/stdlib.h>

#include <cc/x86/instruction.h>

namespace cc {
	namespace x86 {
		enum EFunctionAttribs {
			kFuncAttrib_External = 0,
			kFuncAttrib_Static = 1
		};

		class Function {
		public:
			static Function make_function(const cc::String& name, cc::u32 func_attribs);
			
			void add_instruction(const Instruction& ins);
			void add_instructions(const cc::Array<Instruction>& instructions);

			InstructionsCollection& get_instructions()
				{ return m_instructions; }
		private:
			cc::String m_name;
			cc::x86::InstructionsCollection m_instructions;
			cc::u32 m_attribs;
		};
	}
}
