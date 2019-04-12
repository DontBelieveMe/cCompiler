#include <cc/file_io.h>

#include <cassert>
#include <cstdio>

cc::file cc::read_file(const char* filepath, cc::file::read_mode mode)
{
	cc::file file;
	file.open(filepath, mode);

	if(file.is_open())
	{
		file.read();
	}

	return file;
}

void cc::file::open(const char* filepath, file::read_mode mode)
{
	const char* mode_string = (mode == cc::file::read_mode::binary) ? "rb" : "r";

	m_filehandle = std::fopen(filepath, mode_string);
	m_readmode = mode;	
	m_open = (m_filehandle != nullptr);

	if(!m_filehandle)
	{
		std::fclose(m_filehandle);
	}
	else
	{
		std::fseek(m_filehandle, 0, SEEK_END);	
		m_filesize = std::ftell(m_filehandle);
		std::fseek(m_filehandle, 0, SEEK_SET);
	}
}

void cc::file::read()
{
	if(m_open)
	{
		long alloc_size = m_filesize;

		if (m_readmode == read_mode::text)
		{
			alloc_size = alloc_size + 1;
		}

		m_data = new u8[alloc_size];

		std::size_t read_bytes = std::fread(m_data, sizeof(u8), m_filesize, m_filehandle);

		if (m_readmode == read_mode::text)
		{
			m_data[read_bytes] = 0;
		}

		m_buffersize = read_bytes;
	}
}

void cc::file::close()
{
	if(m_open)
	{
		delete [] m_data;
		m_data = nullptr;
		m_buffersize = 0;
		std::fclose(m_filehandle);
		m_open = false;
	}
}
