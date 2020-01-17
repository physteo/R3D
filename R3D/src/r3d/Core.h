#pragma once

#ifdef R3D_PLATFORM_WIN
	#ifdef R3D_BUILD_DLL
		#define R3D_API __declspec(dllexport)
	#else
		#define R3D_API __declspec(dllimport)
	#endif
#else
	#error R3D supports only Windows.
#endif