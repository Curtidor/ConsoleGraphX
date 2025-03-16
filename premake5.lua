workspace "ConsoleGraphX"
    architecture "x64"
    configurations { "Debug", "Release" }
    startproject "Editor"  -- Start with Editor

    outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

    include "WinCore"
    include "ConsoleGraphX"
    include "Editor"
    include "RunTime"
    include "WindowHandler"
    include "SandBox"