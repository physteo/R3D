#pragma once

#include <random>

namespace r3d
{

	class Random
	{
	public:
		static void init()
		{
			s_randomEngine.seed(std::random_device()());
		}
		
		static float randFloat()
		{
			return (float)s_randomDistribution(s_randomEngine) / (float)std::numeric_limits<uint32_t>::max();
		}
		
		static int randInt(int a, int b)
		{
			assert(b > a);
			return (int) (randFloat() * (b - a) + a);
		}

	private:
		static std::mt19937 s_randomEngine;
		static std::uniform_int_distribution<std::mt19937::result_type> s_randomDistribution;

	};

}
