#include <cc/logging.h>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

using namespace cc;

std::shared_ptr<spdlog::logger> Logger::s_logger;

void Logger::Startup()
{
	spdlog::set_pattern("%^[%T] %n: %v%$");
	s_logger = spdlog::stdout_color_mt("cCompiler");
	s_logger->set_level(spdlog::level::trace);
}
