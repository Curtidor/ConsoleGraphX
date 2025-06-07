newoption {
    trigger     = "withEditor",
    description = "Build with Editor support",
}

function SetEditorDefines()
    if _OPTIONS["withEditor"] then
        defines { "Editor" }
    else
        defines {"RunTime"}
    end
end
