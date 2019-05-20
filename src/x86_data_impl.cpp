// This is hand coded and implements more complex functionality relating to the
// the instruction set data.
//
// Do **not** delete this file & please include it in source control.

#include <cc/x86_data.h>
#include <cstring>
#include <cctype>
#include <cassert>

using namespace cc;

static int StringCompareIgnoreCase(const char* a, const char* b)
{
	// C Standard Library defines this as UB so just assert it's OK.
	assert(a != nullptr);
	assert(b != nullptr);

	// This is mimicking GCC's behavior for when one input is empty.
	// Not sure if this is allowed to be UB or not. If it can be
	// then we may just be able to assert this instead.
	if (a[0] == 0)
		return -1;

	if (b[0] == 0)
		return 1;

	const std::size_t len = std::strlen(a);
	for (std::size_t i = 0; i < len; ++i)
	{
		unsigned char ca = static_cast<unsigned char>(a[i]);
		unsigned char cb = static_cast<unsigned char>(b[i]);

		// Normalize to lowercase if the character is an uppercase ASCII
		// digit.
		// A lot faster than just a `std::tolower` call (yes I've profiled it)
		// This trick brings this function up to roughly the same speed as a normal
		// `std:strcmp`
		if (ca >= 'A' && ca <= 'Z')
			ca |= 0x20;

		if (cb >= 'A' && cb <= 'Z')
			cb |= 0x20;

		if (ca != cb)
			return ca < cb ? -1 : 1;
	}

	return 0;
}

X86Instruction* X86InstructionSet::GetInstructionFromName(const char* name)
{
	// This performs a binary search on all the instructions in the ISA, trying to find
	// one that has a matching mnemonic. The s_instructions list is guaranteed to be
	// sorted so we can do a binary search.

	int min = 0;
	int max = NumberOfInstructions - 1;
	int middle = (max + min) / 2;

	X86Instruction* instruction_ptr = &s_instructions[middle];
	int compare_result = StringCompareIgnoreCase(instruction_ptr->Name(), name);

	while(compare_result != 0)
	{
		if(compare_result < 0)
		{
			min = middle + 1;
		}
		else
		{
			max = middle - 1;
		}

		if(max < min)
		{
			instruction_ptr = nullptr;
			break;
		}

		middle = (max + min) / 2;
		instruction_ptr = &s_instructions[middle];
		compare_result = StringCompareIgnoreCase(instruction_ptr->Name(), name);
	}

	return instruction_ptr;
}

X86InstructionForm* X86Instruction::ResolveForm(const std::vector<EX86Operand>& operand_types)
{
	// Do simple linear search on all forms of an instruction in order to find the smallest
	// instruction that matches the given operands.

	X86InstructionForm* optimal_form = nullptr;

	for(std::size_t j = 0; j < m_forms.size(); ++j)
	{
		X86InstructionForm* form = &m_forms[j];

		// Find if this instruction form matches the given operands.
		bool matches = true;

		if(form->NumOperands() == static_cast<u8>(operand_types.size()))
		{
			const std::array<EX86Operand, 5>& operands = form->Operands();

			for(u8 i = 0; i < form->NumOperands(); ++i)
			{
				if(operands[i] != operand_types[i])
				{
					matches = false;
				}
			}
		}
		else
		{
			matches = false;
		}

		// If this instruction form has the correct operands
		// Check if this instruction form is smallest and if it is
		// then mark it as the smallest instruction so far.
		// (also do this if no matching instruction has been found yet).
		if (matches)
		{
			if((optimal_form && form->InstructionSize() < optimal_form->InstructionSize()) ||
				!optimal_form)
			{
				optimal_form = form;
			}
		}
	}

	return optimal_form;
}
