#include <catch2/catch.hpp>

#include <cc/x86_data.h>

TEST_CASE("X86InstructionSet GetInstructionFromName() can find instruction that exists", "[X86InstructionSet]")
{
	using namespace cc;
	
	SECTION("Find's 'MOV' instruction (uppercase instruction mnemonic)")
	{
		X86Instruction* mov_inst = X86InstructionSet::GetInstructionFromName("MOV");
		
		REQUIRE(mov_inst->InstructionEnum() == EX86Instruction::Mov);
	}

	SECTION("Find's 'Vstmxcsr' instruction (mixed case instruction mnemonic)")
	{
		X86Instruction* vstmxcsr = X86InstructionSet::GetInstructionFromName("Vstmxcsr");

		REQUIRE(vstmxcsr->InstructionEnum() == EX86Instruction::Vstmxcsr);
	}

	SECTION("Find's 'bswap' instruction (lowercase instruction mnemonic)")
	{
		X86Instruction* bswap = X86InstructionSet::GetInstructionFromName("bswap");

		REQUIRE(bswap->InstructionEnum() == EX86Instruction::Bswap);
	}
}

TEST_CASE("X86InstructionSet GetInstructionFromName() return's null pointer for instructions that don't exist", "[X86InstructionSet]")
{
	using namespace cc;

	X86Instruction* inst = X86InstructionSet::GetInstructionFromName("defo_not_a_real_instruction");

	REQUIRE(inst == nullptr);
}

TEST_CASE("Can resolve valid instructions operands to form", "[X86Instruction]")
{
	using namespace cc;

	SECTION("Resolves mov r32/imm32 operand pair")
	{
		X86Instruction* mov_inst = X86InstructionSet::GetInstructionFromName("mov");

		X86InstructionForm* form = mov_inst->ResolveForm({EX86Operand::R32, EX86Operand::Imm32});

		auto operands = form->Operands();

		REQUIRE(operands[0] == EX86Operand::R32);
		REQUIRE(operands[1] == EX86Operand::Imm32);
		REQUIRE(form->OpcodeBytes()[0] == 0xB8);
	}
}

TEST_CASE("Resolves a instruction that doesn't have the given operands to nullptr", "[X86Instruction]")
{
	using namespace cc;

	X86Instruction* bswap = X86InstructionSet::GetInstructionFromName("bswap");

	X86InstructionForm* form = bswap->ResolveForm({EX86Operand::Imm32, EX86Operand::Imm32});
	REQUIRE(form == nullptr);
}
