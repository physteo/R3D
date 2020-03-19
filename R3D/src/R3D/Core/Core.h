#pragma once

//!< Fixes the framerate of the application
//#define R3D_FIXED_FRAMERATE 60.0

#if defined(R3D_DEBUG) || defined(R3D_RELEASE)
	//!< If not defined, asserts are replaced with nothingness
	#define R3D_ENABLE_ASSERTS
#endif

#if defined(R3D_DEBUG) || defined(R3D_RELEASE)
	//!< Application will have a debugMode for extra log calls.
	#define R3D_DEBUG_APP
#endif
//#define R3D_DEBUG_EVENTS

#ifdef R3D_ENABLE_ASSERTS
	#ifdef R3D_PLATFORM_WIN
		#define R3D_ASSERT(x, ...)      { if(!(x)) { R3D_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
		#define R3D_CORE_ASSERT(x, ...) { if(!(x)) { R3D_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
	#elif defined(R3D_PLATFORM_LINUX)
		#define R3D_ASSERT(x, ...)      { if(!(x)) { R3D_ERROR("Assertion Failed: {0}", __VA_ARGS__); assert(false); } }
		#define R3D_CORE_ASSERT(x, ...) { if(!(x)) { R3D_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); assert(false); } }
	#endif
#else
	#define R3D_ASSERT(x, ...)
	#define R3D_CORE_ASSERT(x, ...)
#endif

#ifdef R3D_PLATFORM_WIN
	#ifdef R3D_DYNAMIC_LIB
		#ifdef R3D_BUILD_DLL
			#define R3D_API __declspec(dllexport)
		#else
			#define R3D_API __declspec(dllimport)
		#endif
	#else
		#define R3D_API 
	#endif
#else
        #define R3D_API
#endif
