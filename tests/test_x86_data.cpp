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
