#pragma once

#include "int_types.h"

#include <ctime>

namespace cc
{
	class DateTime
	{
	private:
		std::time_t m_unix_time;

	public:
		DateTime(std::time_t unix_time);
		DateTime();
		
		std::time_t UnixTime() const { return m_unix_time; }

		static DateTime Now();
	};
}
