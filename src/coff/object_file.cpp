#define _CRT_SECURE_NO_WARNINGS

#include <cc/coff/object_file.h>

#include <cc/file_io.h>
#include <cc/mem_utils.h>

#include <numeric>

using namespace cc::coff;

ObjectFile::ObjectFile()
	: m_machine(kMachineUnknown), m_datetime(0) {
}

ObjectFile& ObjectFile::set_machine(EMachineType machine) {
	m_machine = machine;
	return *this;
}

void ObjectFile::read_from_file(const cc::String& filepath) {
	using namespace cc;

	File obj_file_handle(filepath);
	u8* file_data = obj_file_handle.read();

	m_machine = static_cast<EMachineType>(extract_u16(file_data, 0));
	
	u16 num_sections = extract_u16(file_data, 2);
	m_datetime = extract_u32(file_data, 4);
	
	u32 ptr_to_symbol_table = extract_u32(file_data, 8);
	u32 number_of_symbols = extract_u32(file_data, 12);

	u16 optional_header_size = extract_u16(file_data, 16);
	u16 characteristics = extract_u16(file_data, 18);
	
	const size_t kFileHeaderSize = 20;
	const size_t kSectionHeaderSize = 40;

	m_sections.reserve(num_sections);
	for (u16 i = 0; i < num_sections; ++i) {
		SharedPtr<Section> csection = make_shared<Section>(kFileHeaderSize + file_data + (i * kSectionHeaderSize));
		m_sections.push_back(csection);
	}

	m_symbol_table = cc::make_shared<SymbolTable>(file_data + ptr_to_symbol_table, number_of_symbols);
}

void ObjectFile::write_to_file(const cc::String& filepath) {
	// 4 two byte entry's and 3 4 byte entry's make up the header.
	// see below for the exact entry names, size and their offsets.
	const cc::u32 kHeaderSize = (2 * 4) + (4 * 3);
	
	// One section header is 40 bytes in size.
	const cc::u32 kSectionHeadersTableSize = m_sections.size() * 40;

	const cc::u32 kRawDataSize = std::accumulate(m_sections.begin(), m_sections.end(), 0, 
		[](cc::u32 acu, const cc::SharedPtr<cc::coff::Section>& section) -> cc::u32 {
			return acu + section->raw_data().size();
		}
	);

	// The symbol table is to immediately precede the raw data.
	const cc::u32 kSymbolTableOffset = kHeaderSize + kSectionHeadersTableSize + kRawDataSize;

	cc::File file_handle(filepath);	
		
	// #todo (bwilks) - this should be replaced with some custom buffer class :-
	//                  will mean we can easily push back values of any size.
	cc::Array<cc::u8> buffer;
	
	buffer.resize(kHeaderSize); // this means that we can use memset to easily set multi byte values.

	const cc::u16 machine_u16 = static_cast<cc::u16>(m_machine);

	// Machine, Offset: 0, Size: 2 bytes
	std::memcpy(buffer.data(), &machine_u16, 2);

	const cc::u16 num_sections_u16 = static_cast<cc::u16>(m_sections.size());

	// Number of Sections, Offset: 2, Size: 2 bytes
	std::memcpy(buffer.data() + 2, &num_sections_u16, 2);

	// Date Time Stamp, Offset 4, Size: 4 bytes
	std::memcpy(buffer.data() + 4, &m_datetime, 4);

	// Symbol Table Offset/Pointer, Offset 8, Size 4 bytes.
	// NB: This is filled in later, so it falls after each sections raw data.
	std::memset(buffer.data() + 8, 0, 4);

	const cc::u32 num_symbols_u32 = m_symbol_table->symbols().size();

	// Number of Symbols, Offset 12, Size 4 bytes.
	std::memcpy(buffer.data() + 12, &num_symbols_u32, 4);

	// Optional Header Size, Offset 16, Size 2 bytes.
	// NB: This is always 0 for object files.
	std::memset(buffer.data() + 16, 0, 2);
	
	// Characteristics/Attributes, Offset 18, Size 2 bytes.
	// NB: #todo (bwilks), this won't always be 0, and so should be amended to use m_characteristics
	std::memset(buffer.data() + 18, 0, 2);

	size_t raw_dat_offset = kHeaderSize + kSectionHeadersTableSize;
	
	// Calculate the offsets for the raw data of each section.
	// Also add the sections table to the buffer.
	for (cc::SharedPtr<section>& section : m_sections) {
		section->set_raw_data_offset(raw_dat_offset);
		
		cc::Array<cc::u8> section_header = section->gen_section_header_bytes();
		buffer.insert(buffer.end(), section_header.begin(), section_header.end());
	
		raw_dat_offset += section->raw_data().size();
	}

	// Add in each each sections raw data after the sections table.
	for (cc::SharedPtr<section>& section : m_sections) {
		cc::Array<cc::u8> dat = section->raw_data();
		buffer.insert(buffer.end(), dat.begin(), dat.end());
	}

	// Fill in the offset to the start of the symbol table.
	// This is the same as raw_dat_offset as the symbol table
	// directly follows section raw data.
	std::memcpy(buffer.data() + 8, &raw_dat_offset, 4);
	
	// Add the symbol table to the object file, directly after any raw data.
	cc::Array<cc::u8> symbol_table_buffer = m_symbol_table->write_to_buffer();
	buffer.insert(buffer.end(), symbol_table_buffer.begin(), symbol_table_buffer.end());

	// #todo (bwilks) - fix.
	// this is inserts the size of the string table (immediately follows the symbol table)
	// currently there is no string table so just hard code 0 (4 bytes).
	// even if the string table is empty this still needs to be here.
	for (int i = 0; i < 4; i++) buffer.push_back(0);

	file_handle.write(buffer.data(), buffer.size());
}

