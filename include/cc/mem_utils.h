#pragma once

#include <cc/stdlib.h>

#define extract_int(type, byte_buff, byte_offset) \
	*((type*)(byte_buff + byte_offset))

#define extract_u16(byte_buff, byte_offset) \
	extract_int(cc::u16, byte_buff, byte_offset)

#define extract_u32(byte_buff, byte_offset) \
	extract_int(cc::u32, byte_buff, byte_offset)

namespace cc {
	void print_data_as_hex(cc::u8* start, cc::size_t len, size_t max_line_len = 10, int indent = 0);

	/**
	 * @brief Convert a string to a non-null terminated u8 buffer.
	 * @param str The string to convert into a char buffer.
	 * @return A non null terminated char buffer.
	 */
	cc::Array<u8> string_to_char_array(const std::string& str);
}