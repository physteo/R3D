#pragma once

#include <memory>

#include "Core.h"
#include "spdlog/spdlog.h"

namespace r3d
{
	class R3D_API Log
	{
	public:
		
		static void init();
		
		inline static std::shared_ptr<spdlog::logger>& getCoreLogger()   { return s_coreLogger; } 
		inline static std::shared_ptr<spdlog::logger>& getClientLogger() { return s_clientLogger; }

	private:
		static std::shared_ptr<spdlog::logger> s_coreLogger;
		static std::shared_ptr<spdlog::logger> s_clientLogger;
	};
}

// logging macros
#define R3D_CORE_TRACE(...) ::r3d::Log::getCoreLogger()->trace(__VA_ARGS__)
#define R3D_CORE_INFO(...)  ::r3d::Log::getCoreLogger()->info(__VA_ARGS__)
#define R3D_CORE_WARN(...)  ::r3d::Log::getCoreLogger()->warn(__VA_ARGS__)
#define R3D_CORE_ERROR(...) ::r3d::Log::getCoreLogger()->error(__VA_ARGS__)
#define R3D_CORE_CRITICAL(...) ::r3d::Log::getCoreLogger()->critical(__VA_ARGS__)

#define R3D_TRACE(...)      ::r3d::Log::getClientLogger()->trace(__VA_ARGS__)
#define R3D_INFO(...)       ::r3d::Log::getClientLogger()->info(__VA_ARGS__)
#define R3D_WARN(...)       ::r3d::Log::getClientLogger()->warn(__VA_ARGS__)
#define R3D_ERROR(...)      ::r3d::Log::getClientLogger()->error(__VA_ARGS__)
#define R3D_CRITICAL(...)   ::r3d::Log::getClientLogger()->critical(__VA_ARGS__)
