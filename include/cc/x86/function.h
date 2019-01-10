#include <cc/stdlib.h>

#include <cc/x86/instruction.h>

namespace cc {
	namespace x86 {
		class function {
		public:
			static function make_function(const cc::string& name);
			
			void add_instruction(const instruction& ins);
			void add_instructions(const cc::array<instruction>& instructions);

			instructions_collection& get_instructions()
				{ return m_instructions; }
		private:
			cc::string m_name;
			cc::x86::instructions_collection m_instructions;
		};
	}
}
