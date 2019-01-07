#include <cc/coff/section.h>
#include <cc/mem_utils.h>

using namespace cc::coff;

section::section(cc::u8* sections_data) {
	m_name.resize(8);
	std::memcpy(&m_name[0], sections_data, 8);

	m_virtual_size = extract_u32(sections_data, 8);
	m_virtual_addr = extract_u32(sections_data, 12);
	
	cc::u32 raw_data_size = extract_u32(sections_data, 16);
	cc::u32 raw_data_ptr = extract_u32(sections_data, 20);

	m_raw_data.resize(raw_data_size);
	std::memcpy(m_raw_data.data(), (sections_data - 20) + raw_data_ptr, raw_data_size);

	m_reloc_ptr = extract_u32(sections_data, 24);
	m_line_numbers_ptr = extract_u32(sections_data, 28);
	m_num_relocs = extract_u16(sections_data, 32);
	m_num_line_numbers = extract_u16(sections_data, 34);
	m_characteristics = extract_u32(sections_data, 36);
}

section::section()
	: m_characteristics(0) {
}

cc::array<cc::u8> section::gen_section_header_bytes() {
	const cc::size_t kSectionHeaderSize = 8 + (4 * 7) + (2 * 2); // 40, broken down into corresponding element sizes.
	
	cc::array<cc::u8> buff(kSectionHeaderSize);
	
	// Zero the name buffer first (because the name may not be the full 8 bytes and
	// any section names with less than 8 characters should have the rest of the memory
	// zeroed as per the standard.
	std::memset(buff.data(), 0, 8);
	
	// Section Name, Offset: 0, Size: 8 bytes.
	std::memcpy(buff.data(), &m_name[0], 8);

	// #todo(bwilks) - fix
	// None of these are used at the minute so just zero them.
	// eventually they'll need to be looked at more.
	m_virtual_size = 0; // Should be zero for object files.
	m_virtual_addr = 0; // Should be set to zero by compilers (that's us :D)
	m_reloc_ptr = 0;
	m_line_numbers_ptr = 0;
	m_num_line_numbers = 0;
	m_num_relocs = 0;
	
	// Virtual Size (total size of section when loaded into memory). Offset: 8, Size: 4
	// Should be zero for object files.
	std::memcpy(buff.data() + 8, &m_virtual_size, 4);

	// Virtual Address (Address of first byte before relocations are applied). Offset: 12, Size 4
	// Compilers should set this to zero for simplicity.
	std::memcpy(buff.data() + 12, &m_virtual_addr, 4);
	
	const cc::u32 raw_dat_size = m_raw_data.size();
	const cc::u32 raw_dat_ptr = m_raw_data_offset;
	
	// Raw Data Size. Offset: 16, Size 4
	std::memcpy(buff.data() + 16, &raw_dat_size, 4);

	// Raw Data Offset/Pointer. Offset: 20, Size: 4
	// #todo(bwilks). For best performance this should be aligned on a 4 byte boundary.
	//                - not currently enforced
	std::memcpy(buff.data() + 20, &raw_dat_ptr, 4);

	// Relocation entries offset/pointer. Offset: 24, Size: 4
	std::memcpy(buff.data() + 24, &m_reloc_ptr, 4);

	// Line numbers entries offset/pointer. Offset: 24, Size: 4
	std::memcpy(buff.data() + 28, &m_line_numbers_ptr, 4);

	// Number of relocations. Offset: 32, Size 2.
	std::memcpy(buff.data() + 32, &m_num_relocs, 2);

	// Number of line number entries. Offset: 34, Size: 2
	std::memcpy(buff.data() + 34, &m_num_line_numbers, 2);

	// Section characteristics/attributes/flags. Offset: 36, Size: 4
	std::memcpy(buff.data() + 36, &m_characteristics, 4);

	return buff;
}