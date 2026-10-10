-- Guard so multiple dofile() calls don't re-declare things
if _CGX_COMMON_LOADED then return end
_CGX_COMMON_LOADED = true

newoption {
    trigger     = "withEditor",
    description = "Build with Editor support",
}

-- Global Debug/Release filters (apply once at workspace)
function ApplyStandardConfigs()
    filter "configurations:Debug"
        defines {"TERMLOG=1"}
        defines { "DEBUG" }
        symbols "On"
    filter "configurations:Release"
        defines { "NDEBUG" }
        optimize "On"
    filter {}
end

function ApplyBuildModeDefine()
    defines { "MIN_BUILD=0" }
    if _OPTIONS["withEditor"] then
        defines { "Editor" }
    else
        defines { "RunTime" }
    end
end


-- Copies the repo's Assets folder into the output dir after build, so asset
-- paths like "Sprites/wall_sprite.cxsp" resolve relative to the executable.
function CopyAssets()
    postbuildcommands {
        "{COPYDIR} %[%{wks.location}/Assets] %[%{cfg.targetdir}]"
    }
end
