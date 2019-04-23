#pragma once

#include "int_types.h"

namespace cc
{
	class CoffObjectFile
	{
	private:
		u16	m_machine, m_num_sections;
		u32 m_datetime_created;
		u32 m_symbol_table_ptr, m_num_symbols;
		u16 m_characteristics;

	public:
		void ReadFromFile(const char* filepath);
		void WriteToFile(const char* filepath);
	};
}
