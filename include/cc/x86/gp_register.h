#pragma once

#include <cc/stdlib.h>

namespace cc {
	namespace x86 {
		class gp_register {
		public:
			gp_register(cc::u8 encoding);
			gp_register(){}
			static const gp_register eax;
			static const gp_register ecx;
			static const gp_register edx;
			static const gp_register ebx;
			static const gp_register esp;
			static const gp_register ebp;
			static const gp_register esi;
			static const gp_register edi;

			bool operator==(const gp_register& right) const;
			bool operator!=(const gp_register& right) const;

			cc::u8 encoding() const { return m_encoding; }
		private:
			cc::u8 m_encoding;
		};
	}
}
