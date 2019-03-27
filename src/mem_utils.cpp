#define _CRT_SECURE_NO_WARNINGS
#include <cc/mem_utils.h>
#include <cc/logging.h>

void cc::print_data_as_hex(cc::u8* start, cc::size_t len, size_t max_line_len, int indent) {
	cc::FixedArray<char, 3> hex;

	for (int j = 0; j < indent; ++j)
		cc::Console::print(cc::Env::tab);

	cc::size_t col = 0;
	for (cc::size_t i = 0; i < len; ++i) {
		if (col >= max_line_len) {
			cc::Console::println();
			
			for (int j = 0; j < indent; ++j)
				cc::Console::print(cc::Env::tab);
			col = 0;
		}

		std::sprintf(hex.data(), "%02X", *(start + i));
		cc::Console::printf("{0} ", hex.data());
		col++;
	}
	cc::Console::println();
}

cc::Array<cc::u8> cc::string_to_char_array(const std::string& str) {
	return cc::Array<cc::u8>(str.begin(), str.end());
}