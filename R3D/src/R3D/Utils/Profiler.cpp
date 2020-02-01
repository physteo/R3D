#include <R3Dpch.h>

#include "Profiler.h"

namespace r3d
{

	Profiler* Profiler::s_instance = nullptr;
	std::unordered_map<std::string, std::pair<unsigned int, long long> > Profiler::m_data;
	Profiler::timetype Profiler::m_start;

	Profiler::Profiler()
	{
		s_instance = this;
	}

	void Profiler::startCollect()
	{
		m_start = std::chrono::steady_clock::now();
	}

	void Profiler::stopCollect(const std::string& s)
	{
		timetype end = std::chrono::steady_clock::now();
		m_data[s].first++;
		m_data[s].second = std::chrono::duration_cast<nanoseconds>(end - m_start).count();
	}

	void Profiler::print()
	{
		R3D_CORE_INFO("Profiler results.");
		for (auto data : m_data)
		{
			R3D_CORE_INFO("Average time for {0}: {1}", data.first, data.second.second / double(data.second.first));
		}
	}

}