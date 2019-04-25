#include <cc/coff/coff.h>
#include <cc/file_io.h>
#include <cc/buffer.h>

#include <cstdio>
#include <cassert>

using namespace cc;

void CoffObjectFile::ReadFromFile(const char* filepath)
{
	File obj_file = cc::ReadFile(filepath, File::EIOMode::ReadBinary);
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
	m_datetime_created = DateTime(static_cast<std::time_t>(time_date));
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

	// ---------------------------------------------------

	for(u16 i = 0; i < num_sections; ++i)
	{
		SectionHeader section;
		section.ReadFromMemory(data_buffer);

		m_sections.push_back(section);
	}
}

void CoffObjectFile::WriteToFile(const char* filepath)
{
	assert(m_num_sections <= 96);

	WriteBuffer buffer;
	
	buffer.WriteNext<u16>(static_cast<u16>(m_machine));
	buffer.WriteNext<u16>(m_num_sections);
	buffer.WriteNext<u32>(static_cast<u32>(m_datetime_created.UnixTime()));
	buffer.WriteNext<u32>(m_symbol_table_ptr);
	buffer.WriteNext<u32>(m_num_symbols);
	buffer.WriteNext<u16>(0);
	buffer.WriteNext<u16>(m_characteristics);

	cc::WriteFile(
		filepath, cc::File::EIOMode::WriteBinary,
		buffer.Data(), buffer.Size()
	);
}
