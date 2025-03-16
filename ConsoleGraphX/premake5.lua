project "ConsoleGraphX"
    kind "StaticLib"
    language "C++"
    cppdialect "C++20"
    staticruntime "on"

    targetdir ("../bin/%{cfg.buildcfg}")
    objdir ("../bin-int/%{cfg.buildcfg}")

    files { "**.cpp", "**.h" } 


    pchheader "PCH_CGX.h"  
    pchsource "Engine/Core/PCH/PCH_CGX.cpp" 

    includedirs { ".", "../WinCore", "Engine/Core/PCH" } 
    links { "WinCore" }

    filter "configurations:Debug"
        symbols "on"

    filter "configurations:Release"
        optimize "on"
