#include <cc/coff/symbols.h>
#include <cc/assert.h>

#include <cstring>

using namespace cc::coff;

/*
	symbol_type u16 representation:
		LSB -> base_type
		MSB -> complex_type
*/

cc::u16 SymbolType::get_type() {
	return base_type | (complex_type << 4);
}

static const cc::size_t kMaxSymbolNameLength = 8;

SymbolName::SymbolName(cc::String str) {
	// #todo (bwilks) fix this.
	// requires implementation of the string table, so ties in with other todos.
	ASSERT(str.length() <= kMaxSymbolNameLength, "Only symbol names with a length <= 8 characters are supported.")

	std::memset(&m_data, 0, kMaxSymbolNameLength);
	std::memcpy(&m_data, str.data(), str.length());
}

SymbolName SymbolName::from_buff(cc::u8* buff) {
	SymbolName name;
	std::memcpy(&name.m_data, buff, kMaxSymbolNameLength);
	return name;
}

SymbolType::SymbolType(const cc::u16& type) {
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

bool SymbolName::uses_short_name() {
	return m_data.long_name.zeroes != 0;
}

cc::String SymbolName::get_short_name() {
	return cc::String(m_data.short_name, m_data.short_name + kMaxSymbolNameLength);
}

cc::u32 SymbolName::get_long_name_string_table_offset() {
	return m_data.long_name.offset;
}

cc::Array<cc::u8> Symbol::write_to_buffer() {
	const cc::size_t kSymbolHeaderSize = 18;

	cc::Array<cc::u8> buffer;
	buffer.resize(kSymbolHeaderSize);
	
	ASSERT(name.uses_short_name(), "Symbol names must be <= 8 characters, so that the symbol name can always be encoded in the symbol table.");

	cc::String name_str = name.get_short_name();
	
	std::memcpy(buffer.data(), name_str.data(), 8);
	std::memcpy(buffer.data() + 8, &value, 4);
	std::memcpy(buffer.data() + 12, &section_number, 2);

	cc::u16 sym_type = type.get_type();
	std::memcpy(buffer.data() + 14, &sym_type, 2);

	std::memcpy(buffer.data() + 16, &storage_clss, 1);
	std::memcpy(buffer.data() + 17, &number_of_aux_symbols, 1);

	return buffer;
}

Symbol Symbol::from_buff(cc::u8* buff) {
	Symbol sym;
	
	// Symbol Name, Offset: 0, Size: 8
	sym.name = SymbolName::from_buff(buff);
	
	// Value, Offset: 8, Size: 4
	// How this value is interpreted depends on the Section Number and
	// the Storage Class of this symbol. Typically it is the relocatable
	// address.
	std::memcpy(&sym.value, buff + 8, 4);

	// Section Number, Offset: 12, Size: 2
	// 1-based index into the section table, to denote the section this
	// symbol belongs to.
	// Some values have special meaning (defined in 5.4.2 PE Format MSDN Documentation)
	std::memcpy(&sym.section_number, buff + 12, 2);
	
	// Type, Offset: 14, Size 2
	cc::u16 symbol_type_tmp = 0;
	std::memcpy(&symbol_type_tmp, buff + 14, 2);
	sym.type = SymbolType(symbol_type_tmp);

	// Storage Class, Offset: 16, Size: 1
	std::memcpy(&sym.storage_clss, buff + 16, 1);

	// Number Of Preceding Auxillary Records, Offset: 17, Size: 1
	std::memcpy(&sym.number_of_aux_symbols, buff + 17, 1);
	
	return sym;
}

SymbolTable::SymbolTable(cc::u8* read_buff, cc::u32 nsymbols) {
	const cc::u32 kSymbolRecordSize = 18; // bytes
	
	m_symbols.reserve(nsymbols);
	
	cc::u32 aux_records_since_last_normal_record = 0;
	cc::u32 symbol_record_offset = 0;

	for (cc::u32 i = 0; i < nsymbols; ++i) {
		bool is_aux_record = false;
		
		// Check if this is an auxillary record
		if (aux_records_since_last_normal_record > 0) {
			aux_records_since_last_normal_record--;
			is_aux_record = true;
		}
		
		Symbol sym = Symbol::from_buff(read_buff + symbol_record_offset);
		sym.is_aux = is_aux_record;
		
		// If this record has any preceding auxillary records then, store how many
		// so we can identify if a record is auxillary or not.
		if (sym.number_of_aux_symbols > 0) {
			aux_records_since_last_normal_record = sym.number_of_aux_symbols;
		}

		m_symbols.push_back(sym);

		// Advance the offset into the symbol table so we are reading
		// the next record on the next iteration.
		symbol_record_offset += kSymbolRecordSize;
	}
}

cc::Array<cc::u8> SymbolTable::write_to_buffer()
{
	cc::Array<cc::u8> buff;
	
	for (Symbol& sym : m_symbols) {
		cc::Array<cc::u8> sym_buff = sym.write_to_buffer();
		ASSERT(sym_buff.size() == 18, "Symbol defintion buffer must be 18 bytes in size")
		
		buff.insert(buff.end(), sym_buff.begin(), sym_buff.end());
	}

	return buff;
}
