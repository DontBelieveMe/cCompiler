#pragma once

#include "int_types.h"

#include <cstdio>

namespace cc
{
	class File
	{
	public:
		enum class EIOMode
		{
			ReadBinary,
			ReadText,
			WriteBinary,
			WriteText
		};

		void Open(const char* filepath, File::EIOMode mode);
		void Read();
		void Write(u8* data, std::size_t size);
		void Close();
		
		/// Return the read file data (only valid if read(...) has been called, if not nullptr will be returned)
		u8* Data() { return m_data; }
	
		/// Return if the file is currently open. False if tried to open a file that doesn't exist.
		bool IsOpen() const { return m_open; }

		/// Return the size of buffer containing the file data.
		std::size_t DataSize() { return m_buffersize; }

		/// Return the size of the file as it is on disk.
		std::size_t FileSize() { return m_filesize; }

	private:
		u8*         m_data        = nullptr;
		bool        m_open        = false;
		std::FILE*  m_filehandle  = nullptr;
		long        m_filesize    = 0;
		std::size_t m_buffersize  = 0;
		EIOMode   m_readmode;
	};

	File ReadFile(const char* filepath, File::EIOMode mode);
	void WriteFile(const char* filepath, File::EIOMode mode, u8* data, std::size_t size);
}
