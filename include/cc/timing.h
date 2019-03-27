#pragma once

#include <cc/stdlib.h>

#include <functional>
#include <chrono>

namespace cc {
	class TimeBlock {
	public:
		TimeBlock(const std::function<void(float)>& end_callback);
		~TimeBlock();

	private:
		std::chrono::steady_clock::time_point m_start;		
		std::function<void(float)> m_end_callback;
	};
}
