project "WinCore"
    kind "StaticLib"
    language "C++"
    cppdialect "C++20"
    staticruntime "on"

    targetdir ("../bin/%{cfg.buildcfg}")
    objdir ("../bin-int/%{cfg.buildcfg}")


    files { "**.cpp", "**.h" } 

    debugdir "%{cfg.targetdir}"

    pchheader "PCH_WCore.h"  
    pchsource "PCH_WCore.cpp" 

    includedirs { "." } 

    filter "configurations:Debug"
        defines { "DEBUG" }
        symbols "On"

    filter "configurations:Release"
        defines { "NDEBUG" }
        optimize "On"
