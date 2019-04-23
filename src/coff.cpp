#include <cc/coff.h>
#include <cc/file_io.h>
#include <cc/buffer.h>

#include <cstdio>
#include <cassert>

using namespace cc;

void CoffObjectFile::ReadFromFile(const char* filepath)
{
	File obj_file = cc::ReadFile(filepath, File::EReadMode::Binary);
	ReadBuffer data_buffer(obj_file.Data(), obj_file.DataSize());

	const u16 machine          = data_buffer.ReadNext<u16>();
	const u16 num_sections     = data_buffer.ReadNext<u16>();
	const u32 time_date        = data_buffer.ReadNext<u32>();
	const u32 symbol_table_ptr = data_buffer.ReadNext<u32>();
	const u32 num_symbols      = data_buffer.ReadNext<u32>();
	const u16 opt_header_size  = data_buffer.ReadNext<u16>();
	const u16 characteristics  = data_buffer.ReadNext<u16>();
	
	m_machine          = static_cast<ECoffMachineType>(machine);
	m_num_sections     = num_sections;
	m_datetime_created = time_date;
	m_symbol_table_ptr = symbol_table_ptr;
	m_num_symbols      = num_symbols;
	m_characteristics  = characteristics;

	// ----------------------------------------------------
	// Basic sanity checks, according to constraints set by
	// COFF standard
	// ----------------------------------------------------
	
	assert(opt_header_size == 0);
	assert(num_sections <= 96);
	assert(m_machine == ECoffMachineType::I386);

	struct SectionHeader
	{
		u8 Name[8];
		u8 _[32];
	};

	static_assert (
		sizeof(SectionHeader) == 40,
		"SectionHeader must be 40 bytes in size."
	);

	for(u16 i = 0; i < num_sections; ++i)
	{
		SectionHeader header = data_buffer.ReadNext<SectionHeader>();
		std::printf("Section Header Name: %s\n", header.Name);
	}
}

void CoffObjectFile::WriteToFile(const char* filepath)
{
}
