#pragma once

#include "int_types.h"

#include <cstdio>

namespace cc
{
	class file
	{
	public:
		enum class read_mode
		{
			binary,
			text
		};

		void open(const char* filepath, file::read_mode mode);
		void read();
		void close();
		
		/// Return the read file data (only valid if read(...) has been called, if not nullptr will be returned)
		u8* data() { return m_data; }
		
		/// Return if the file is currently open. False if tried to open a file that doesn't exist.
		bool is_open() const { return m_open; }

		/// Return the size of buffer containing the file data.
		std::size_t data_size() { return m_buffersize; }

		/// Return the size of the file as it is on disk.
		std::size_t file_size() { return m_filesize; }

	private:
		u8* m_data = nullptr;
		bool m_open = false;
		std::FILE* m_filehandle = nullptr;
		long m_filesize = 0;
		std::size_t m_buffersize = 0;
		read_mode m_readmode;
	};

	file read_file(const char* filepath, file::read_mode mode);
}
