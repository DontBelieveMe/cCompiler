#pragma once

#include <cc/stdlib.h>
#include <unordered_map>
#include <cc/x86/instruction.h>
namespace cc {
	struct Instruction {
		std::uint8_t primary_opcode;
		
		std::uint8_t pp;
		std::uint8_t mm;
		cc::size_t mod;
		cc::x86::EInstructionOperandsDef operands;
	};

	class InstructionSet
	{
	private:
		static std::unordered_map<const char*, Instruction> m_instructions;

	public:
		static const Instruction& get_instruction_by_mnemonic(const char* mnemonic)
		{
			return m_instructions.at(mnemonic);
		}
	};
}
