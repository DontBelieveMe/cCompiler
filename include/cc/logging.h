#pragma once

#include <spdlog/logger.h>
#include <memory>

#define CTRACE(...) cc::logger::get_spdlog_logger()->trace(__VA_ARGS__)
#define CDEBUG(...) cc::logger::get_spdlog_logger()->debug(__VA_ARGS__)
#define CINFO(...) cc::logger::get_spdlog_logger()->info(__VA_ARGS__)
#define CWARN(...) cc::logger::get_spdlog_logger()->warn(__VA_ARGS__)
#define CERROR(...) cc::logger::get_spdlog_logger()->error(__VA_ARGS__)
#define CFATAL(...) cc::logger::get_spdlog_logger()->critical(__VA_ARGS__)

namespace cc {
	class logger {
	private:
		static std::shared_ptr<spdlog::logger> s_logger;

	public:
		static void startup();

		static const std::shared_ptr<spdlog::logger>& get_spdlog_logger() { return s_logger; }
	};
}