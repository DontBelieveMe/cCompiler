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

	SECTION("Resolves only one where only one form exists.")
	{
		X86Instruction* mov_inst = X86InstructionSet::GetInstructionFromName("maskmovq");

		X86InstructionForm* form = mov_inst->ResolveForm({EX86Operand::Mm, EX86Operand::Mm});

		auto operands = form->Operands();

		REQUIRE(operands[0] == EX86Operand::Mm);
		REQUIRE(operands[1] == EX86Operand::Mm);
		REQUIRE(form->OpcodeBytes()[0] == 0xf);
		REQUIRE(form->OpcodeBytes()[1] == 0xf7);
	}

	SECTION("Can resolve the smallest instruction when two forms have the same operands")
	{
		// here the assembler has to choose between
		//	"mov r/m32, imm32" (6 bytes) (0xC7)
		// and
		//	"mov r32, imm32" (5 bytes) (0xB8)
		X86Instruction* mov_inst = X86InstructionSet::GetInstructionFromName("mov");

		X86InstructionForm* form = mov_inst->ResolveForm({ EX86Operand::R32, EX86Operand::Imm32 });

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
