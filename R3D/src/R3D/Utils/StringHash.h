#pragma once

#include <R3D/Core/Core.h>

#ifdef R3D_PLATFORM_LINUX
#include <inttypes.h>
#endif

namespace r3d
{

	class StringHash
	{
	public:
		static unsigned long long computeHash(const std::string& str);

	};

	namespace murmur
	{
		//-----------------------------------------------------------------------------
		// MurmurHash2, 64-bit versions, by Austin Appleby
		// Note - This code makes a few assumptions about how your machine behaves -
		// 1. We can read a 4-byte value from any address without crashing
		// 2. sizeof(int) == 4
		// And it has a few limitations -
		// 1. It will not work incrementally.
		// 2. It will not produce the same results on little-endian and big-endian
		//    machines.
#if defined(R3D_PLATFORM_WIN)
		typedef unsigned __int64 uint64_t;
#endif
		// 64-bit hash for 64-bit platforms
		uint64_t MurmurHash64A(const void * key, int len, unsigned int seed);

		// 64-bit hash for 32-bit platforms
		uint64_t MurmurHash64B(const void * key, int len, unsigned int seed);
	}

}
