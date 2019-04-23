#ifdef _WIN32
	#define _CRT_SECURE_NO_WARNINGS
#endif

#include <cc/file_io.h>

#include <cassert>
#include <cstdio>

using namespace cc;

void cc::WriteFile(const char* filepath, File::EIOMode mode, u8* data, std::size_t size)
{
	cc::File file;
	file.Open(filepath, mode);

	if(file.IsOpen())
	{
		file.Write(data, size);
	}
}

cc::File cc::ReadFile(const char* filepath, cc::File::EIOMode mode)
{
	cc::File file;
	file.Open(filepath, mode);

	if(file.IsOpen())
	{
		file.Read();
	}

	return file;
}

void cc::File::Open(const char* filepath, File::EIOMode mode)
{
	const char* mode_string = nullptr;
	switch(mode)
	{
	case EIOMode::ReadBinary: mode_string = "rb"; break;
	case EIOMode::ReadText: mode_string = "r"; break;
	case EIOMode::WriteBinary: mode_string = "wb"; break;
	case EIOMode::WriteText: mode_string = "w"; break;
	default: mode_string = nullptr;
	}

	assert(mode_string);

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

void cc::File::Write(u8* data, std::size_t size)
{
	assert(m_readmode == EIOMode::WriteText || m_readmode == EIOMode::WriteBinary);

	if(m_open)
	{
		std::fwrite(data, sizeof(cc::u8), size, m_filehandle);
	}
}

void cc::File::Read()
{
	assert(m_readmode == EIOMode::ReadText || m_readmode == EIOMode::ReadBinary);

	if(m_open)
	{
		long alloc_size = m_filesize;

		if (m_readmode == EIOMode::ReadText)
		{
			alloc_size = alloc_size + 1;
		}

		m_data = new u8[alloc_size];

		std::size_t read_bytes = std::fread(m_data, sizeof(u8), m_filesize, m_filehandle);

		if (m_readmode == EIOMode::ReadText)
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
