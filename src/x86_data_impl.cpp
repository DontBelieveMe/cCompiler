// This is hand coded and implements more complex functionality relating to the
// the instruction set data.
//
// Do **not** delete this file & please include it in source control.

#include <cc/x86_data.h>
#include <cstring>
#include <cctype>
#include <cassert>

using namespace cc;

static int StringCompareIgnoreCase(const char* a, const char* b) {
  assert(a != nullptr);
  assert(b != nullptr);

  if (a[0] == 0) return -1;

  if (b[0] == 0) return 1;

  const std::size_t len = std::strlen(a);
  for (std::size_t i = 0; i < len; ++i) {
    unsigned char ca = static_cast<unsigned char>(a[i]);
    unsigned char cb = static_cast<unsigned char>(b[i]);

	if (ca >= 'A' && ca <= 'Z') ca |= 0x20; //std::tolower(ca);

	if (cb >= 'A' && cb <= 'Z') cb |= 0x20;  std::tolower(cb);

	if (ca != cb) return ca - cb; // ca < cb ? -1 : 1;
  }

  return 0;
}

/*
/// Mimics the behavior of strcmp however treats every character as
/// a lowercase version of itself.
/// e.g StringCompareIgnoreCase("aBcD", "ddCC") == StringCompareIgnoreCase("abcd", "ddcc")
static int StringCompareIgnoreCase(const char* a, const char* b)
{
	assert(a != nullptr);
	assert(b != nullptr);

	const std::size_t a_len = std::strlen(a);
	const std::size_t b_len = std::strlen(b);
	
	// note (bdw): This is just mimicking what GCC does when passing
	//             empty strings to strlen.
	if (a_len == 0)
		return -1;

	if (b_len == 0)
		return 1;

	for(std::size_t i = 0; i < a_len; ++i)
	{
		const unsigned char ca = std::tolower(static_cast<unsigned char>(a[i]));
		const unsigned char cb = std::tolower(static_cast<unsigned char>(b[i]));

		if (ca != cb)
			return ca < cb ? -1 : 1;
	}

	return 0;
}*/

X86Instruction* X86InstructionSet::GetInstructionFromName(const char* name)
{
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
