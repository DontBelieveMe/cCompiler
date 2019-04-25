#pragma once

#include "int_types.h"

#include <cassert>
#include <cstring>

namespace cc
{
	class WriteBuffer
	{
	private:
		u8 *m_buffer;
		std::size_t m_size;
		std::size_t m_capacity;

	public:
		WriteBuffer(std::size_t initial_size);
		WriteBuffer();
		~WriteBuffer();

		std::size_t Size() const { return m_size; }
		std::size_t Capacity() const { return m_capacity; }
		
		u8* Data() const { return m_buffer; }

		template <typename T>
		void WriteNext(T value)
		{
			if(m_capacity >= m_size)
			{
				if(m_capacity == 0)
					m_capacity = 16;
				else
					m_capacity *= 2;
				
				u8* old_buffer = m_buffer;
				m_buffer = new u8[m_capacity];
				
				if(old_buffer)
				{
					std::memcpy(m_buffer, old_buffer, m_size);
					delete [] old_buffer;
				}
			}

			*(reinterpret_cast<T*>(m_buffer) + m_size) = value;
			m_size += sizeof(T);
		}
	};

	class ReadBuffer
	{
	private:
		u8         *m_ptr;
		u8         *m_start;

		std::size_t m_count;
	public:
		ReadBuffer(u8* buffer, std::size_t count);
		ReadBuffer();

		void Clear();
		void ClearAndSet(u8* buffer, std::size_t count);

		void ResetToStart();
		
		template <typename T>
		T ReadNext()
		{
			const T value = PeekNext<T>();
			m_ptr += sizeof(T);
			
			return value;
		}

		template <typename T>
		T PeekNext()
		{
			const int TYPE_SIZE = sizeof(T);

			// todo (bwilks): need better handling here
			assert(((m_ptr + TYPE_SIZE) - m_start) < static_cast<std::ptrdiff_t>(m_count));

			const T value = *(reinterpret_cast<T*>(m_ptr));

return value;
		}

		bool SequenceEqualsString(const char* str);

		void Advance(std::size_t advance_by);
	};
}
