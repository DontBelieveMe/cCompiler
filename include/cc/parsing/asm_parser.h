#pragma once

#include <cc/stdlib.h>

namespace cc {
	namespace parsing {
		class asm_file {
		public:
			static asm_file from_file(const cc::string& filepath);
		private:
		};
	}
}
