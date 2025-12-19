project("app")
    kind("ConsoleApp")
    language("C++")
    cppdialect("C++20")
    targetdir("binaries/%{cfg.buildcfg}")
    staticruntime("off")
    
    files({ "source/**.h", "source/**.cpp" })
    
    includedirs({
        "source",
        "../core/source",
        "../cli/source",
        "/usr/local/include",
    })
    
    -- Put static library FIRST in linkoptions, BEFORE links
    linkoptions({
        "-Wl,--whole-archive",
        "/usr/local/lib/libpqxx.a",
        "-Wl,--no-whole-archive",
    })
    
    links({
        "core",
        "cli",
        "pq",
        "pthread",
    })
    
    targetdir("../binaries/" .. OutputDir .. "/%{prj.name}")
    objdir("../binaries/intermediates/" .. OutputDir .. "/%{prj.name}")
    
    filter("system:windows")
        systemversion("latest")
        defines({ "WINDOWS" })
    
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
