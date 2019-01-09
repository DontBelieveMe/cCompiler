#include <cc/timing.h>
#include <cc/logging.h>

using namespace cc;

time_block::time_block(const std::function<void(float)>& end_callback)
	: m_end_callback(end_callback) {
	
	m_start = std::chrono::high_resolution_clock::now();
}

time_block::~time_block() {
	auto end = std::chrono::high_resolution_clock::now();
	std::chrono::duration<float> delta_seconds = end - m_start;
	m_end_callback(delta_seconds.count());
}
