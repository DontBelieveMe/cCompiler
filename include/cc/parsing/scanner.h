#pragma once

#include <cc/stdlib.h>

namespace cc {
	namespace parsing {
		// #todo (bwilks) -> finish this when it comes to parsing C
		enum token_type {
			kTok_LParan,
			kTok_RParan
		};
		
		struct token {
		};

		class scanner {
		public:
			static scanner from_file(const cc::String& filepath);


		private:
			cc::String m_string;
		};
	}
}
