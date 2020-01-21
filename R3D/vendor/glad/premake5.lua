project "glad"
	kind "StaticLib"
	language "C"

	includedirs {"include"}

	files
	{
		"src/glad.c"
	}

	filter "system:windows"
		systemversion "latest"
		staticruntime "On"

		defines 
		{ 
			"_CRT_SECURE_NO_WARNINGS"
		}

	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		runtime "Release"
		optimize "on"
