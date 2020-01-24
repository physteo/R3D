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

IncludeDir = {}
IncludeDir["GLFW"] = "R3D/vendor/GLFW/include"
IncludeDir["glad"] = "R3D/vendor/glad/include"
IncludeDir["imgui"] = "R3D/vendor/imgui"
IncludeDir["glm"] = "R3D/vendor/glm/glm"

group "Dependencies"
   include "R3D/vendor/GLFW"
   include "R3D/vendor/glad"
   include "R3D/vendor/imgui"

-- adds here thepremake inside R3D/vendor/GLFW
include "R3D/vendor/glad"
include "R3D/vendor/GLFW"
include "R3D/vendor/imgui"

project "R3D"
   location "R3D"
   kind "StaticLib"
   language "C++"
   cppdialect "C++17"
   staticruntime "on"


   targetdir ("bin/" .. outputdir .. "/%{prj.name}")
   objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

   pchheader "R3Dpch.h"
   pchsource "R3D/src/R3Dpch.cpp"

   files 
   {
      "%{prj.name}/src/**.h",
      "%{prj.name}/src/**.cpp"
   }

   includedirs
   {
      "%{prj.name}/vendor/spdlog/include",
      "%{prj.name}/src",
      "%{IncludeDir.GLFW}",
      "%{IncludeDir.glad}",
      "%{IncludeDir.imgui}",
      "%{IncludeDir.glm}"
   }
   

   links
   {
      "glad",
      "GLFW",
      "imgui"
   }

   filter "system:linux"
     systemversion "latest"

     makesettings [[
         CXX = g++-7
     ]]

     defines
     {
        "R3D_PLATFORM_LINUX",
        "GLFW_INCLUDE_NONE"
     }

     links
     {
        "GL",
        "dl",
        "X11",
        "pthread"
     }

   filter "system:windows"
     systemversion "latest"

     defines
     {
        "R3D_PLATFORM_WIN",
       "GLFW_INCLUDE_NONE"
     }

     links
     {
        "opengl32.lib"
     }

   filter "configurations:Debug"
      defines "R3D_DEBUG"
      symbols "on"
           
   filter "configurations:Release"
      defines "R3D_RELEASE"
      optimize "on"
           
   filter "configurations:Dist"
      defines "R3D_DIST"
      optimize "on"

   filter {"system:windows", "configurations:Release"}
      buildoptions "/MT"


project "Sandbox"
   location "Sandbox"
   kind "ConsoleApp"
   language "C++"
   cppdialect "C++17"

   staticruntime "on"

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
      "R3D/vendor/spdlog/include",
      "%{IncludeDir.GLFW}",
      "%{IncludeDir.glad}",
      "%{IncludeDir.imgui}",
      "%{IncludeDir.glm}"
   }
   

   filter "system:linux"
      systemversion "latest"

      makesettings [[
         CXX = g++-7
      ]]

      linkgroups "on"   
   
      links
      {
         "glad",
         "GLFW",
         "imgui",
         "GL",
         "dl",
         "X11",
         "pthread",
         "R3D"
      }


   filter "system:windows"
      systemversion "latest"

      defines
      {
        "R3D_PLATFORM_WIN"
      }

   links
   {
      "R3D"
   }

   filter "configurations:Debug"
      defines "R3D_DEBUG"
      symbols "on"
           
   filter "configurations:Release"
      defines "R3D_RELEASE"
      optimize "on"
           
   filter "configurations:Dist"
      defines "R3D_DIST"
      optimize "on"
