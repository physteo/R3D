#pragma once

#include <R3D/Core/Log.h>

namespace r3d
{
	class Profiler
	{
		using timetype = std::chrono::steady_clock::time_point;
		using nanoseconds = std::chrono::nanoseconds;

		static std::unordered_map<std::string, std::pair<unsigned int, long long> > m_data;
		static timetype m_start;

	public:
		Profiler();

		static void startCollect();
		static void stopCollect(const std::string& s);
		static void print();

		static Profiler* s_instance;

	};
}