#pragma once

#include "../int_types.h"
#include "../buffer.h"

#include <string>

namespace cc
{
	class SectionName
	{
	private:
		union
		{
			u8 short_name[9];
			u32 string_table_offset;
		} m_name;

		bool m_uses_short_form;
	public:
		SectionName();

		void ReadFromMemory(ReadBuffer& buffer);

		bool IsInStringTable() const
			{ return !m_uses_short_form; }

		std::string ResolveString(/* String Table */) const;
	};

	class SectionHeader
	{
	private:
		SectionName m_name;
		u32 m_vsize;
		u32 m_vaddr;
		u32 m_rawdata_size;
		u32 m_rawdata_ptr;
		u32 m_reloc_ptr;
		u32 m_linenumbers_ptr;
		u16 m_num_relocs;
		u16 m_num_linenumbers;
		u32 m_characteristics;

	public:
		void ReadFromMemory(ReadBuffer& buffer);
		void WriteToMemory(WriteBuffer& buffer);

		const SectionName& Name() const
			{ return m_name; }

		u32 VirtualSize()         const { return m_vsize;           }
		u32 VirtualAddress()      const { return m_vaddr;           }
		u32 RawDataSize()         const { return m_rawdata_size;    }
		u32 RawDataPtr()          const { return m_rawdata_ptr;     }
		u32 RelocationsPtr()      const { return m_reloc_ptr;       }
		u32 LineNumbersPtr()      const { return m_linenumbers_ptr; }
		u16 NumberOfRelocations() const { return m_num_relocs;      }
		u16 NumberOfLineNumbers() const { return m_num_linenumbers; }
		u32 Characteristics()     const { return m_characteristics; }
	};
}
