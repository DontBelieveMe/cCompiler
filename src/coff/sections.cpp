#include <cc/coff/sections.h>
#include <cc/logging.h>

#include <cassert>
#include <cstring>
#include <cstdlib>

#include <algorithm>

using namespace cc;

SectionName::SectionName()
	: m_uses_short_form(true)
{
	std::memset(&m_name, 0, sizeof(m_name));
}

std::string SectionName::ResolveString() const
{
	assert(m_uses_short_form);

	return std::string(m_name.short_name, m_name.short_name + 8);
}

void SectionName::ReadFromMemory(ReadBuffer& buffer)
{
	u8 name_buffer[8];

	for(int i = 0; i < 8; ++i)
		name_buffer[i] = buffer.ReadNext<u8>();

	if (name_buffer[0] == '/')
	{
		m_uses_short_form = false;

		std::rotate(name_buffer, name_buffer + 1, name_buffer + 8);
		name_buffer[7] = '\0';

		const char* num_string = (const char*) name_buffer;
		m_name.string_table_offset = std::atoi(num_string);
	}
	else
	{
		m_uses_short_form = true;

		std::memcpy(m_name.short_name, name_buffer, 8);
		m_name.short_name[8] = '\0';
	}
}

void SectionHeader::ReadFromMemory(ReadBuffer& buffer)
{
	m_name.ReadFromMemory(buffer);

	// Do not support the string table for now...
	assert(!m_name.IsInStringTable());

	m_vsize           = buffer.ReadNext<u32>();
	m_vaddr           = buffer.ReadNext<u32>();
	m_rawdata_size    = buffer.ReadNext<u32>();
	m_rawdata_ptr     = buffer.ReadNext<u32>();
	m_reloc_ptr       = buffer.ReadNext<u32>();
	m_linenumbers_ptr = buffer.ReadNext<u32>();
	m_num_relocs      = buffer.ReadNext<u16>();
	m_num_linenumbers = buffer.ReadNext<u16>();
	m_characteristics = buffer.ReadNext<u32>();

	/*
	 * Basic sanity checks
	 */

	// This should be 0 for object files
	assert(m_vsize == 0);
}

void SectionHeader::WriteToMemory(WriteBuffer& buffer)
{
}
