#pragma once

#include "int_types.h"
#include <cassert>

namespace cc
{
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
