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

   filter "system:linux"
      systemversion "latest"
      makesettings [[
         CXX = g++-7
      ]]


   filter "configurations:Debug"
      runtime "Debug"
      symbols "on"

   filter "configurations:Release"
      runtime "Release"
      optimize "on"

   filter "configurations:Dist"
      runtime "Release"
      optimize "on"
