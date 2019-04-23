#pragma once

#include <cstddef>
#include <cstring>

namespace cc
{
	class StringView
	{
	private:
		const char* m_str;
		std::size_t m_count;

	public:
		StringView() noexcept
			: m_str(nullptr), m_count(0)
		{ }

		StringView(const StringView& other)
		{
			m_str = other.m_str;
			m_count = other.m_count;
		}

		StringView(const char* s, std::size_t count)
			: m_str(s), m_count(count)
		{ }

		StringView(const char* s)
			: m_str(s)
		{
			m_count = std::strlen(s);
		}
	};
}
