#pragma once

#include "../int_types.h"
#include "../datetime.h"

#include "sections.h"

#include <vector>

namespace cc
{
	enum class ECoffMachineType : u16;

	class CoffObjectFile
	{
	private:
		ECoffMachineType m_machine;
		u16              m_num_sections;
		DateTime         m_datetime_created;
		u32              m_symbol_table_ptr;
		u32              m_num_symbols;
		u16              m_characteristics;

		std::vector<SectionHeader> m_sections;

	public:
		void Machine(ECoffMachineType machine) { m_machine = machine; }
		ECoffMachineType Machine() const { return m_machine; }

		void TimeStamp(DateTime timestamp) { m_datetime_created = timestamp; }
		DateTime TimeStamp() const { return m_datetime_created; }

		void ReadFromFile(const char* filepath);
		void WriteToFile(const char* filepath);

		const std::vector<SectionHeader>& Sections() const
			{ return m_sections; }
	};

	enum class ECoffMachineType : u16
	{
		Unknown   = 0x0,
		AM33      = 0x1D3,
		AMD64     = 0x8664,
		ARM       = 0x1C0,
		ARM64     = 0xAA64,
		ARMNT     = 0x1C4,
		EBC       = 0xEBC,
		I386      = 0x14C,
		IA64      = 0x200,
		M32R      = 0x9041,
		MIPS16    = 0x266,
		MIPSFPU   = 0x366,
		MIPSFPU16 = 0x466,
		PowerPC   = 0x1F0,
		PowerPCFP = 0x1F1,
		RV4000    = 0x166,
		RISCV32   = 0x5032,
		RISCV64   = 0x5064,
		RISCV128  = 0x5128,
		SH3       = 0x1A2,
		SH3DSP    = 0x1A3,
		SH4       = 0x1A6,
		SH5       = 0x1A8,
		Thumb     = 0x1C2,
		WCEMIPSV2 = 0x169
	};
}
