project "SandBox"
    kind "SharedLib"
    language "C++"
    cppdialect "C++20"
    staticruntime "on"

    targetdir ("../bin/%{cfg.buildcfg}")
    objdir ("../bin-int/%{cfg.buildcfg}")

    files { "**.h", "**.cpp" }

    debugdir "%{cfg.targetdir}"
    
    includedirs { "../WinCore", "../ConsoleGraphX" }
    links { "WinCore", "ConsoleGraphX" }

    libdirs { "../bin/%{cfg.buildcfg}" }


    filter "configurations:Debug"
        defines { "DEBUG" }
        symbols "Full"
        optimize "Off"

    filter "configurations:Release"
        defines { "NDEBUG" }
        optimize "On"
