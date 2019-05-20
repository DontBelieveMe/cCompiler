#include <cc/x86_encoder.h>

using namespace cc;

static void EmitOpcodes(X86InstructionForm* form, WriteBuffer& buffer)
{
	const auto& opcodes = form->OpcodeBytes();

	for (u8 i = 0; i < form->NumOpcodeBytes(); ++i)
		buffer.WriteNext<u8>(opcodes[i]);
}

static bool IsOperandTypeImmediate(EX86Operand operand_type)
{
	return operand_type == EX86Operand::Imm8 ||
		operand_type == EX86Operand::Imm16 ||
		operand_type == EX86Operand::Imm32 ||
		operand_type == EX86Operand::Imm4;
}

#define operand_type_cast(type, operand) *((type*)operand)

static void EmitImmediateData(X86InstructionForm* form, const std::vector<void*>& operands, WriteBuffer& buffer)
{
	const auto& operand_types = form->Operands();
	const u8 num_operands = form->NumOperands();

	for (u8 i = 0; i < num_operands; ++i)
	{
		if (IsOperandTypeImmediate(operand_types[i]))
		{
			switch (operand_types[i])
			{
			case EX86Operand::Imm8:
				buffer.WriteNext<u8>(operand_type_cast(u8, operands[i]));
				break;
			case EX86Operand::Imm16:
				buffer.WriteNext<u16>(operand_type_cast(u16, operands[i]));
				break;
			case EX86Operand::Imm32:
				buffer.WriteNext<u32>(operand_type_cast(u32, operands[i]));
				break;
			default:
				assert(!"Invalid immediate operand type.");
			}

			return;
		}
	}
}

void X86Encoder::Encode(X86InstructionForm* form, const std::vector<void*>& operands, WriteBuffer& buffer)
{
	EmitOpcodes(form, buffer);
	EmitImmediateData(form, operands, buffer);
}
