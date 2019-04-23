#include <cc/datetime.h>

using namespace cc;

DateTime::DateTime(std::time_t unix_time)
	: m_unix_time(unix_time)
{}

DateTime::DateTime()
	: m_unix_time(0)
{}

DateTime DateTime::Now()
{
	return DateTime(std::time(0));
}
