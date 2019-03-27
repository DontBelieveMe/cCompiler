#pragma once

#include <string>
#include <cc/stdlib.h>

namespace cc {
	enum EFileType {
		kFileModeText,
		kFileModeBinary
	};

	class File {
	public:
		File(const std::string& filepath);
		
		u8 *read(EFileType type = kFileModeBinary);
		void write(u8* data, size_t size, EFileType = kFileModeBinary);
		u8 *data() { return m_data.data(); }
		bool exists() { return m_exists; }

		cc::Array<u8>& data_array() { return m_data; }

	private:
		cc::Array<u8> m_data;
		bool m_exists;
		cc::String m_filepath;
	};
}
