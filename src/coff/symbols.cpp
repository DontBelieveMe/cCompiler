#include <cc/coff/symbols.h>
#include <cc/assert.h>

#include <cstring>

using namespace cc::coff;

/*
	symbol_type u16 representation:
		LSB -> base_type
		MSB -> complex_type
*/

cc::u16 symbol_type::get_type() {
	return base_type | (complex_type << 4);
}

static const cc::size_t kMaxSymbolNameLength = 8;

symbol_name::symbol_name(cc::string str) {
	// #todo (bwilks) fix this.
	// requires implementation of the string table, so ties in with other todos.
	ASSERT(str.length() <= kMaxSymbolNameLength, "Only symbol names with a length <= 8 characters are supported.")

	std::memset(&m_data, 0, kMaxSymbolNameLength);
	std::memcpy(&m_data, str.data(), str.length());
}

symbol_name symbol_name::from_buff(cc::u8* buff) {
	symbol_name name;
	std::memcpy(&name.m_data, buff, kMaxSymbolNameLength);
	return name;
}

symbol_type::symbol_type(const cc::u16& type) {
	/*
		#define N_BTMASK                            0x000F
		#define N_TMASK                             0x0030
		...
		#define N_BTSHFT                            4
		
		// Basic Type of  x
		#define BTYPE(x) ((x) & N_BTMASK)

		...

		// Is x a function?
		#ifndef ISFCN
		#define ISFCN(x) (((x) & N_TMASK) == (IMAGE_SYM_DTYPE_FUNCTION << N_BTSHFT))
		#endif

		And hence origin of following magic numbers...
		#todo: properly abstract these out.
	*/
	base_type = type & 0x000F;
	complex_type = (type & 0x0030) >> 4;
}

bool symbol_name::uses_short_name() {
	return m_data.long_name.zeroes != 0;
}

cc::string symbol_name::get_short_name() {
	return cc::string(m_data.short_name, m_data.short_name + kMaxSymbolNameLength);
}

cc::u32 symbol_name::get_long_name_string_table_offset() {
	return m_data.long_name.offset;
}

cc::array<cc::u8> symbol::write_to_buffer() {
	const cc::size_t kSymbolHeaderSize = 18;

	cc::array<cc::u8> buffer;
	buffer.resize(kSymbolHeaderSize);
	
	ASSERT(name.uses_short_name(), "Symbol names must be <= 8 characters, so that the symbol name can always be encoded in the symbol table.");

	cc::string name_str = name.get_short_name();

	std::memcpy(buffer.data(), name_str.data(), 8);
	std::memcpy(buffer.data() + 8, &value, 4);
	std::memcpy(buffer.data() + 12, &section_number, 2);

	cc::u16 sym_type = type.get_type();
	std::memcpy(buffer.data() + 14, &sym_type, 2);

	std::memcpy(buffer.data() + 16, &storage_clss, 1);
	std::memcpy(buffer.data() + 17, &number_of_aux_symbols, 1);

	return buffer;
}

symbol symbol::from_buff(cc::u8* buff) {
	symbol sym;
	sym.name = symbol_name::from_buff(buff);
	std::memcpy(&sym.value, buff + 8, 4);
	std::memcpy(&sym.section_number, buff + 12, 2);
	u16 t = 0;
	std::memcpy(&t, buff + 14, 2);
	sym.type = symbol_type(t);

	std::memcpy(&sym.storage_clss, buff + 16, 1);
	std::memcpy(&sym.number_of_aux_symbols, buff + 17, 1);
	
	return sym;
}

symbol_table::symbol_table(cc::u8* read_buff, cc::u32 nsymbols) {
	m_symbols.reserve(nsymbols);
	cc::u32 aux = 0;
	cc::u32 offset = 0;
	for (cc::u32 i = 0; i < nsymbols; ++i) {
		bool is_aux = false;
		if (aux > 0) {
			aux--;
			is_aux = true;
		}
		symbol sym = symbol::from_buff(read_buff + offset);
		sym.is_aux = is_aux;
		if (sym.number_of_aux_symbols > 0)
			aux = sym.number_of_aux_symbols;

		m_symbols.push_back(sym);
		offset += 18;
	}
}

cc::array<cc::u8> symbol_table::write_to_buffer()
{
	cc::array<cc::u8> buff;
	
	for (symbol& sym : m_symbols) {
		cc::array<cc::u8> sym_buff = sym.write_to_buffer();
		ASSERT(sym_buff.size() == 18, "Symbol defintion buffer must be 18 bytes in size")
		
		buff.insert(buff.end(), sym_buff.begin(), sym_buff.end());
	}

	return buff;
}
