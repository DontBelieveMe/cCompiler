#pragma once

#include <cc/stdlib.h>

namespace cc {
	namespace coff {
		enum ESectionCharacteristics {
			kImageScn_RESERVED0 = 0x00000000,
			kImageScn_RESERVED1 = 0x00000001,
			kImageScn_RESERVED2 = 0x00000002,
			kImageScn_RESERVED3 = 0x00000004,
			kImageScnTypeNoPad = 0x00000008,
			kImageScn_RESERVED4 = 0x00000010,
			kImageScnCntCode = 0x00000020,
			kImageScnCntInitializedData = 0x00000040,
			kImageScnCntUninitializedData = 0x00000080,
			kImageScnLnkOther = 0x00000100,
			kImageScnLnkInfo = 0x00000200,
			kImageScn_RESERVED5 = 0x00000400,
			kImageScnLnkRemove = 0x00000800,
			kImageScnLnkComdat = 0x00001000,
			kImageScnGprel = 0x00008000,
			kImageScnMemPurgeable = 0x00020000,
			kImageScnMemLocked = 0x00040000,
			kImageScnMemPreload = 0x00080000,
			kImageScnAlign1Bytes = 0x00100000,
			kImageScnAlign2Bytes = 0x00200000,
			kImageScnAlign4Bytes = 0x00300000,
			kImageScnAlign8Bytes = 0x00400000,
			kImageScnAlign16Bytes = 0x00500000,
			kImageScnAlign32Bytes = 0x00600000,
			kImageScnAlign64Bytes = 0x00700000,
			kImageScnAlign128Bytes = 0x00800000,
			kImageScnAlign256Bytes = 0x00900000,
			kImageScnAlign512Bytes = 0x00A00000,
			kImageScnAlign1024Bytes = 0x00B00000,
			kImageScnAlign2048Bytes = 0x00C00000,
			kImageScnAlign4096Bytes = 0x00D00000,
			kImageScnAlign8192Bytes = 0x00E00000,
			kImageScnLnkNrelocOvfl = 0x01000000,
			kImageScnMemDiscardable = 0x02000000,
			kImageScnMemNotCached = 0x04000000,
			kImageScnMemNotPaged = 0x08000000,
			kImageScnMemShared = 0x10000000,
			kImageScnMemExecute = 0x20000000,
			kImageScnMemRead = 0x40000000,
			kImageScnMemWrite = 0x80000000
		};

		class Section {
		public:
			struct reloc_table {
				cc::u32 virtual_addr;
				cc::u32 symbol_table_index;
				cc::u16 type; // #todo ... fill in type indicators for each platform
			};

			Section(cc::u8* sections_data);
			Section();

			const cc::String& name() { return m_name; }
			
			cc::Array<cc::u8>& raw_data() { return m_raw_data; }

			Section& set_name(const cc::String& name) {
				m_name = name;
				return *this;
			}

			Section& set_raw_data(const cc::Array<cc::u8>& data) {
				m_raw_data = data;
				return *this;
			}

			Section& set_characteristics(cc::u32 characteristics) {
				m_characteristics = characteristics;
				return *this;
			}

			Section& set_raw_data_offset(cc::u32 offset) {
				m_raw_data_offset = offset;
				return *this;
			}

			cc::Array<cc::u8> gen_section_header_bytes();

		private:
			cc::String m_name;

			cc::Array<cc::u8> m_raw_data;
			cc::u32 m_raw_data_offset;

			cc::u32 m_virtual_size, m_virtual_addr;
			cc::u32 m_reloc_ptr, m_line_numbers_ptr;
			cc::u16 m_num_relocs, m_num_line_numbers;

			cc::u32 m_characteristics;
		};

	}
}