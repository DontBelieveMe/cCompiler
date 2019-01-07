#pragma once

#include <string>
#include <cc/stdlib.h>

namespace cc {
	class file {
	public:
		file(const std::string& filepath);
		
		u8 *read();
		void write(u8* data, size_t size);
		u8 *data() { return m_data.data(); }
		bool exists() { return m_exists; }
	private:
		cc::array<u8> m_data;
		bool m_exists;
		cc::string m_filepath;
	};
}