project "D3D12Sis"
   kind "ConsoleApp"
   language "C++"
   cppdialect "C++17"
   targetdir "bin/%{cfg.buildcfg}"
   staticruntime "off"

   pchheader "sspch.h"
   pchsource "../%{prj.name}/src/sspch.cpp"
   
   files { "src/**.h", "src/**.cpp" }

   includedirs
   {
      "src",
      "../glfw/include"
   }

   links
   {
      "GLFW"
   }

   targetdir ("../bin/" .. outputdir .. "/%{prj.name}")
   objdir ("../bin-int/" .. outputdir .. "/%{prj.name}")

   filter "system:windows"
      systemversion "latest"
      defines { "SS_PLATFORM_WINDOWS" }
   filter "system:linux"
      systemversion "latest"
      defines { "SS_PLATFORM_LINUX" }

   filter "configurations:Debug"
      defines { "SS_DEBUG" }
      runtime "Debug"
      symbols "On"

   filter "configurations:Release"
      defines { "SS_RELEASE" }
      runtime "Release"
      optimize "On"
      symbols "On"

   filter "configurations:Dist"
      kind "WindowedApp"
      defines { "SS_DIST" }
      runtime "Release"
      optimize "On"
      symbols "Off"