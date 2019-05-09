// This is hand coded and implements more complex functionality relating to the
// the instruction set data.
//
// Do **not** delete this file & please include it in source control.

#include <cc/x86_data.h>
#include <cstring>

using namespace cc;

X86Instruction* X86InstructionSet::GetInstructionFromName(const char* name)
{
	int min = 0;
	int max = NumberOfInstructions - 1;
	int middle = (max + min) / 2;

	X86Instruction* instruction_ptr = &s_instructions[middle];
	int compare_result = std::strcmp(instruction_ptr->Name(), name);

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
		compare_result = std::strcmp(instruction_ptr->Name(), name);
	}

	return instruction_ptr;
}
