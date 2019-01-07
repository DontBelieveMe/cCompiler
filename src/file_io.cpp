#define _CRT_SECURE_NO_WARNINGS
#include <cc/file_io.h>
#include <cc/logging.h>

#include <cstdio>
#include <cc/mem_utils.h>
using namespace cc;

file::file(const std::string& filepath) 
	: m_filepath(filepath) {
}

cc::u8* file::read() {
	std::FILE* file = std::fopen(m_filepath.c_str(), "rb");

	if (file) {
		m_exists = true;
		std::fseek(file, 0, SEEK_END);
		long size = std::ftell(file);
		std::fseek(file, 0, SEEK_SET);

		m_data.resize(size);
		std::fread(m_data.data(), sizeof(u8), size, file);
		std::fclose(file);

		CDEBUG("Read {0} bytes from file {1}", size, m_filepath);
		return m_data.data();
	}
	else {
		m_exists = false;
		CWARN("File {0} does not exist for reading. Returning null.", m_filepath);
		return nullptr;
	}
}

void file::write(u8* data, size_t size) {
	std::FILE *file = std::fopen(m_filepath.c_str(), "wb");
	
	if (!file) {
		CWARN("File {0} does not exist for writing. aborting write.", m_filepath);
		return;
	}

	std::fwrite(data, sizeof(cc::u8), size, file);
	std::fclose(file);

	CDEBUG("Written {0} bytes to file {1}", size, m_filepath);
}
