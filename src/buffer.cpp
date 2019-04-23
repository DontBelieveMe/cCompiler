#include <cc/buffer.h>
#include <cstring>

using namespace cc;

ReadBuffer::ReadBuffer(u8* buffer, std::size_t count)
	: m_ptr(buffer), m_start(buffer), m_count(count)
{
}

ReadBuffer::ReadBuffer()
	: m_ptr(nullptr), m_start(nullptr), m_count(0)
{
}

void ReadBuffer::Clear()
{
	m_ptr = nullptr;
	m_start = nullptr;

	m_count = 0;
}

void ReadBuffer::ClearAndSet(u8* buffer, std::size_t count)
{
	m_ptr = buffer;
	m_start = buffer;

	m_count = 0;
}

void ReadBuffer::ResetToStart()
{
	m_ptr = m_start;
}

bool ReadBuffer::SequenceEqualsString(const char* str)
{
	const std::size_t string_len = std::strlen(str);

	assert((m_ptr + string_len) - m_start < static_cast<std::ptrdiff_t>(m_count));

	const bool equals = std::memcmp(m_ptr, str, string_len) == 0;
	return equals;
}

void ReadBuffer::Advance(std::size_t advance_by)
{
	assert((m_ptr + advance_by) - m_start < static_cast<std::ptrdiff_t>(m_count));
	m_ptr += advance_by;
}
