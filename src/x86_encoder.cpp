#include <cc/x86_encoder.h>

using namespace cc;

void X86Encoder::Encode(X86InstructionForm* form, const std::vector<void*>& operands, WriteBuffer& buffer)
{
	buffer.WriteNext<u8>(0xB8);
	buffer.WriteNext<u32>(0x02);
}
