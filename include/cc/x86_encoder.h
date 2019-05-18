#pragma once

#include <cc/x86_data.h>
#include <cc/buffer.h>

#include <vector>

namespace cc {
	class X86Encoder {
	public:
		void Encode(X86InstructionForm* form, const std::vector<void*>& operands, WriteBuffer& buffer);
	private:
	};
}
