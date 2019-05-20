#include <catch2/catch.hpp>

#include <cc/x86_encoder.h>
#include <cc/buffer.h>

TEST_CASE("Can encode simple single byte instructions", "[X86Encoder]")
{
	using namespace cc;

	X86Encoder encoder;

	X86Instruction* inst = X86InstructionSet::GetInstructionFromName("mov");

	SECTION("mov")
	{
		std::vector<EX86Operand> operand_types = { EX86Operand::R32, EX86Operand::Imm32 };
		X86InstructionForm* form = inst->ResolveForm(operand_types);

		const u32 immediate = 2;
		const X86Register* r32 = &X86Register::Eax;

		std::vector<void*> operands = { (void*)r32, (void*)&immediate };

		WriteBuffer instruction_buffer(form->SizeBytes());
		encoder.Encode(form, operands, instruction_buffer);

		u8* buffer = instruction_buffer.Data();

		REQUIRE(buffer[0] == 0xB8);
		REQUIRE(buffer[1] == 0x02);
		REQUIRE(buffer[2] == 0x00);
		REQUIRE(buffer[3] == 0x00);
		REQUIRE(buffer[4] == 0x00);
	}
}
