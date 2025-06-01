dofile("premake_common.lua")

workspace "ConsoleGraphX"
    architecture "x64"
    configurations { "Debug", "Release" }
    if _OPTIONS["withEditor"] then
        startproject "Editor"
    else
        startproject "RunTime"
    end

    outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

    include "WinCore"
    include "ConsoleGraphX"
    include "Editor"
    include "RunTime"
    include "WindowHandler"
    include "SandBox"