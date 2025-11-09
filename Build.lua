-- premake5.lua
workspace("new project")
architecture("x64")
configurations({ "debug", "release", "dist" })
startproject("app")

-- Workspace-wide build options for MSVC
filter("system:windows")
buildoptions({ "/EHsc", "/Zc:preprocessor", "/Zc:__cplusplus" })

OutputDir = "%{cfg.system}-%{cfg.architecture}/%{cfg.buildcfg}"

group("core")
include("core/Build-Core.lua")
group("")

include("app/Build-App.lua")
