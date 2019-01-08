#pragma once

#include <string>
#include <cc/stdlib.h>

namespace cc {
	enum file_type {
		kFileModeText,
		kFileModeBinary
	};

	class file {
	public:
		file(const std::string& filepath);
		
		u8 *read(file_type type = kFileModeBinary);
		void write(u8* data, size_t size, file_type = kFileModeBinary);
		u8 *data() { return m_data.data(); }
		bool exists() { return m_exists; }

		cc::array<u8>& data_array() { return m_data; }

	private:
		cc::array<u8> m_data;
		bool m_exists;
		cc::string m_filepath;
	};
}
