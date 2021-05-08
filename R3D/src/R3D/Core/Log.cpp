#include <R3Dpch.h>

#include "Log.h"

#pragma warning(push)
#pragma warning(disable: 26451)
#pragma warning(disable: 26812)
#pragma warning(disable: 26495)
#pragma warning(disable: 6385)

#include <spdlog/sinks/stdout_color_sinks.h>

#include <GLFW/glfw3.h>

namespace r3d
{
	
	std::shared_ptr<spdlog::logger> Log::s_coreLogger;
	std::shared_ptr<spdlog::logger> Log::s_clientLogger;

	void Log::init()
	{
		spdlog::set_pattern("%^[%T] %n: %v%$");
		s_coreLogger = spdlog::stdout_color_mt("R3D");
		s_coreLogger->set_level(spdlog::level::trace);

		s_clientLogger = spdlog::stdout_color_mt("APP");
		s_coreLogger->set_level(spdlog::level::trace);
	}
	 
}
 
#pragma warning(pop)