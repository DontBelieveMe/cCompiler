#pragma once

#include <cc/stdlib.h>

#include <functional>
#include <chrono>

namespace cc {
	class time_block {
	public:
		time_block(const std::function<void(float)>& end_callback);
		~time_block();

	private:
		std::chrono::steady_clock::time_point m_start;		
		std::function<void(float)> m_end_callback;
	};
}
