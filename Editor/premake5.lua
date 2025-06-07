dofile("../premake_common.lua")

project "Editor"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++20"
    staticruntime "on"

    SetEditorDefines()

    targetdir ("../bin/%{cfg.buildcfg}")
    objdir ("../bin-int/%{cfg.buildcfg}")

    files { "**.h", "**.cpp" }

    includedirs { "../WinCore", "../ConsoleGraphX" }
    links { "WinCore", "ConsoleGraphX" }

    libdirs { "../bin/%{cfg.buildcfg}" }

    debugdir "%{cfg.targetdir}"

    filter "configurations:Debug"
        defines { "DEBUG" }
        symbols "On"

    filter "configurations:Release"
        defines { "NDEBUG" }
        optimize "On"
