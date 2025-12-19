dofile("../premake_common.lua")

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

    ApplyBuildModeDefine()
