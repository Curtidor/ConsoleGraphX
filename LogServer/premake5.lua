project "LogServer"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++20"
    staticruntime "on"

    targetdir ("../bin/%{cfg.buildcfg}")
    objdir ("../bin-int/%{cfg.buildcfg}")

    files { "**.cpp", "**.h" } 

    debugdir "%{cfg.targetdir}"

    includedirs { "." } 

    ApplyBuildModeDefine()
