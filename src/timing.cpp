#include <cc/timing.h>
#include <cc/logging.h>

using namespace cc;

TimeBlock::TimeBlock(const std::function<void(float)>& end_callback)
	: m_end_callback(end_callback) {
	
	m_start = std::chrono::steady_clock::now();
}

TimeBlock::~TimeBlock() {
	auto end = std::chrono::steady_clock::now();
	std::chrono::duration<float> delta_seconds = end - m_start;
	m_end_callback(delta_seconds.count());
}
