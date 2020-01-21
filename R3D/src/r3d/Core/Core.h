#pragma once

#define R3D_ENABLE_ASSERTS
//!< Fixes the framerate of the application
#define R3D_FIXED_FRAMERATE 60.0

#ifdef R3D_ENABLE_ASSERTS
	#define R3D_ASSERT(x, ...)      { if(!(x)) { R3D_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
	#define R3D_CORE_ASSERT(x, ...) { if(!(x)) { R3D_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
#else
	#define R3D_ASSERT(x, ...)
	#define R3D_CORE_ASSERT(x, ...)
#endif

#ifdef R3D_PLATFORM_WIN
	#ifdef R3D_BUILD_DLL
		#define R3D_API __declspec(dllexport)
	#else
		#define R3D_API 
	#endif
#else
	#error R3D supports only Windows.
#endif