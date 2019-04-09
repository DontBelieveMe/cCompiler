#pragma once

#include <cc/stdlib.h>

namespace cc {
	struct Instruction {
		std::string name;
		std::uint8_t primary_opcode;
		
		std::uint8_t pp;
		std::uint8_t mm;
	};
}
