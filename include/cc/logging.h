#pragma once

#include <spdlog/logger.h>
#include <memory>

#ifdef DEBUG
	#define CTRACE(...) cc::Logger::get_spdlog_logger()->trace(__VA_ARGS__)
	#define CDEBUG(...) cc::Logger::get_spdlog_logger()->debug(__VA_ARGS__)
#else
	#define CTRACE(...)
	#define CDEBUG(...)
#endif

#define CINFO(...) cc::Logger::get_spdlog_logger()->info(__VA_ARGS__)
#define CWARN(...) cc::Logger::get_spdlog_logger()->warn(__VA_ARGS__)
#define CERROR(...) cc::Logger::get_spdlog_logger()->error(__VA_ARGS__)
#define CFATAL(...) cc::Logger::get_spdlog_logger()->critical(__VA_ARGS__)

namespace cc {
	class Logger {
	private:
		static std::shared_ptr<spdlog::logger> s_logger;

	public:
		static void startup();

		static const std::shared_ptr<spdlog::logger>& get_spdlog_logger() { return s_logger; }
	};
}
