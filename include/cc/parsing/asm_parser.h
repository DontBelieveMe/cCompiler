#pragma once

#include <cc/stdlib.h>
#include <cc/x86/instruction.h>

namespace cc {
	namespace parsing {
		class asm_file {
		public:
			static asm_file from_file(const cc::string& filepath);
			cc::x86::instructions_collection& get_instructions() { return m_instructions; }

		private:
			cc::x86::instructions_collection m_instructions;
		};
	}
}
