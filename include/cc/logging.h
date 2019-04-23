#pragma once

#include <spdlog/logger.h>
#include <memory>

#ifdef DEBUG
	#define CTRACE(...) cc::Logger::GetSpdlogLogger()->trace(__VA_ARGS__)
	#define CDEBUG(...) cc::Logger::GetSpdlogLogger()->debug(__VA_ARGS__)
#else
	#define CTRACE(...)((void)0)
	#define CDEBUG(...) ((void)0)
#endif

#define CINFO(...) cc::Logger::GetSpdlogLogger()->info(__VA_ARGS__)
#define CWARN(...) cc::Logger::GetSpdlogLogger()->warn(__VA_ARGS__)
#define CERROR(...) cc::Logger::GetSpdlogLogger()->error(__VA_ARGS__)
#define CFATAL(...) cc::Logger::GetSpdlogLogger()->critical(__VA_ARGS__)

namespace cc
{
	class Logger
	{
	private:
		static std::shared_ptr<spdlog::logger> s_logger;

	public:
		static void Startup();

		static const std::shared_ptr<spdlog::logger>& GetSpdlogLogger()
			{ return s_logger; }
	};
}
