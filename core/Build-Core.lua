project("core")
    kind("StaticLib")
    language("C++")
    cppdialect("C++20")
    targetdir("binaries/%{cfg.buildcfg}")
    staticruntime("off")
    
    files({ "source/**.h", "source/**.cpp" })
    
    includedirs({
        "source",
        "/usr/local/include",  -- Add this for static libpqxx
    })
    
    -- Remove the links block entirely - static libs don't need to link
    -- The final executable will do the linking
    
    targetdir("../binaries/" .. OutputDir .. "/%{prj.name}")
    objdir("../binaries/intermediates/" .. OutputDir .. "/%{prj.name}")
    
    filter("system:windows")
        systemversion("latest")
        defines({})
    
    filter("configurations:debug")
        defines({ "DEBUG" })
        runtime("debug")
        symbols("On")
    
    filter("configurations:release")
        defines({ "RELEASE" })
        runtime("release")
        optimize("On")
        symbols("On")
    
    filter("configurations:dist")
        defines({ "DIST" })
        runtime("release")
        optimize("On")
        symbols("Off")
