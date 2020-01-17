workspace "R3D"
   architecture "x64"
   startproject "Sandbox"

   configurations
   {
   	   "Debug",
	   "Release",
	   "Dist"
   }

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

project "R3D"
   location "R3D"
   kind "SharedLib"
   language "C++"

   targetdir ("bin/" .. outputdir .. "/%{prj.name}")
   objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

   files 
   {
   	   "%{prj.name}/src/**.h",
	   "%{prj.name}/src/**.cpp"
   }

   includedirs
   {
   	   "%{prj.name}/vendor/spdlog/include"
   }

   filter "system:windows"
      cppdialect "C++17"
	  staticruntime "on"
	  systemversion "latest"

	  defines
	  {
	     "R3D_PLATFORM_WIN",
		 "R3D_BUILD_DLL"
	  }

	  postbuildcommands
	  {
	     ("{COPY} %{cfg.buildtarget.relpath} ../bin/" .. outputdir .. "/Sandbox")
	  }

   filter "configurations:Dist"
      defines "R3D_DIST"
	  optimize "on"

   filter "configurations:Release"
      defines "R3D_RELEASE"
	  optimize "on"

   filter "configurations:Debug"
      defines "R3D_DEBUG"
	  symbols "on"

   filter {"system:windows", "configurations:Release"}
      buildoptions "/MT"


project "Sandbox"
   location "Sandbox"
   kind "ConsoleApp"
   language "C++"

   targetdir ("bin/" .. outputdir .. "/%{prj.name}")
   objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

   files 
   {
   	   "%{prj.name}/src/**.h",
	   "%{prj.name}/src/**.cpp"
   }

   includedirs
   {
       "R3D/src",
	   "R3D/vendor/spdlog/include"

   }
   
   links
   {
   	   "R3D"
   }

   filter "system:windows"
      cppdialect "C++17"
	  staticruntime "on"
	  systemversion "latest"

	  defines
	  {
	     "R3D_PLATFORM_WIN"
	  }

   filter "configurations:Dist"
      defines "R3D_DIST"
	  optimize "on"

   filter "configurations:Release"
      defines "R3D_RELEASE"
	  optimize "on"

   filter "configurations:Debug"
      defines "R3D_DEBUG"
	  symbols "on"

   filter {"system:windows", "configurations:Release"}
      buildoptions "/MT"