#pragma once

#include <cc/stdlib.h>

namespace cc {
	enum EOperandType {
		kRegisterOperand,
		kMemoryOperand,
		kRegisterMemoryOperand,
		kImmediateOperand,
		kRelativeOperand
	};

	struct Operand {
		EOperandType type;
	};

	template <int NumOperands, int OpcodeSize>
	struct Instruction {
		Operand operands[NumOperands];
		const char* mnemonic;
		cc::u8 opcode[OpcodeSize];
	};
}
