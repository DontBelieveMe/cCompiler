#ifdef _WIN32
	#define _CRT_SECURE_NO_WARNINGS
#endif

#include <cc/file_io.h>

#include <cassert>
#include <cstdio>

cc::File cc::ReadFile(const char* filepath, cc::File::EReadMode mode)
{
	cc::File file;
	file.Open(filepath, mode);

	if(file.IsOpen())
	{
		file.Read();
	}

	return file;
}

void cc::File::Open(const char* filepath, File::EReadMode mode)
{
	const char* mode_string = (mode == cc::File::EReadMode::Binary) ? "rb" : "r";

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

void cc::File::Read()
{
	if(m_open)
	{
		long alloc_size = m_filesize;

		if (m_readmode == EReadMode::Text)
		{
			alloc_size = alloc_size + 1;
		}

		m_data = new u8[alloc_size];

		std::size_t read_bytes = std::fread(m_data, sizeof(u8), m_filesize, m_filehandle);

		if (m_readmode == EReadMode::Text)
		{
			m_data[read_bytes] = 0;
		}

		m_buffersize = read_bytes;
	}
}

void cc::File::Close()
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
