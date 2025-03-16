project "WindowHandler"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++20"
    staticruntime "on"

    targetdir ("../bin/%{cfg.buildcfg}")
    objdir ("../bin-int/%{cfg.buildcfg}")

    files { "**.h", "**.cpp" }
    includedirs { "../WinCore", "../ConsoleGraphX" }
    links { "WinCore", "ConsoleGraphX" }

    filter "configurations:Debug"
        defines { "DEBUG" }
        symbols "On"

    filter "configurations:Release"
        defines { "NDEBUG" }
        optimize "On"
