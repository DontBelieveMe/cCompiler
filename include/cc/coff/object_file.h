#pragma once

#include <cc/stdlib.h>

#include <cc/coff/section.h>
#include <cc/coff/symbols.h>

namespace cc {
	namespace coff {
		enum machine_type {
			kMachineUnknown = 0x0,
			kMachineAm33 = 0x1d3,
			kMachineAmd64 = 0x8664,
			kMachineArm = 0x1c0,
			kMachineArm64 = 0xaa64,
			kMachineArmNT = 0x1c4,
			kMachineEBC = 0xebc,
			kMachinei386 = 0x14c,
			kMachineia64 = 0x200,
			// #todo: finish these
		};

		enum object_file_characteristics {
			kImageFileRelocsStripped = 0x0001,
			kImageFileExecutableImage = 0x0002,
			kImageFileLineNumsStripped = 0x0004,
			kImageFileLocalSymsStripped = 0x0008,
			kImageFileAggresiveWsTrim = 0x0010,
			kImageFileLargeAddressAware = 0x0020,
			kImageFile_RESERVED0 = 0x0040,
			kImageFileBytesReversedLO = 0x0080,
			kImageFile32bitMachine = 0x0100,
			kImageFileDebugStripped = 0x0200,
			kImageFileRemovableRunFromSwap = 0x0400,
			kImageFileNetRunFromSwap = 0x0800,
			kImageFileSystem = 0x1000,
			kImageFileDll = 0x2000,
			kImageFileUpSystemOnly = 0x4000,
			kImageFileBytesReversedHI = 0x8000
		};

		class object_file {
		public:
			object_file();

			void read_from_file(const cc::string& filepath);

			/**
			 * @brief Writes this COFF/PE object file to disk as a well formed object file, ready for linking.
			 *        e.g. NOT just a straight data dump.
			 * @param filepath 
			 */
			void write_to_file(const cc::string& filepath);

			/**
			 * @brief Set the target machine of this object file
			 * @param machine The new target machine for this object file.
			 * @return A reference to this object_file instance. Useful for chaining setter calls.
			 */
			object_file& set_machine(machine_type machine);

			object_file& set_datetime(cc::u32 datetime) {
				m_datetime = datetime;
				return *this;
			}

			machine_type get_machine() const {
				return m_machine;
			}

			void add_section(const cc::shared_ptr<cc::coff::section>& section) {
				m_sections.push_back(section);
			}

			void set_symbol_table(const std::shared_ptr<symbol_table>& table) {
				m_symbol_table = table;
			}

		private:
			machine_type m_machine;
			cc::u32 m_datetime;
			cc::shared_ptr<symbol_table> m_symbol_table;

			cc::array<cc::shared_ptr<cc::coff::section>> m_sections;
		};
	}
}