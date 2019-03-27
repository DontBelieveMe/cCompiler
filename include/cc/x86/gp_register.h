#pragma once

#include <cc/stdlib.h>

namespace cc {
	namespace x86 {
		class GeneralPurposeRegister {
		public:
			GeneralPurposeRegister(cc::u8 encoding);
			GeneralPurposeRegister(){}
			static const GeneralPurposeRegister eax;
			static const GeneralPurposeRegister ecx;
			static const GeneralPurposeRegister edx;
			static const GeneralPurposeRegister ebx;
			static const GeneralPurposeRegister esp;
			static const GeneralPurposeRegister ebp;
			static const GeneralPurposeRegister esi;
			static const GeneralPurposeRegister edi;

			bool operator==(const GeneralPurposeRegister& right) const;
			bool operator!=(const GeneralPurposeRegister& right) const;

			cc::u8 encoding() const { return m_encoding; }
		private:
			cc::u8 m_encoding;
		};
	}
}
