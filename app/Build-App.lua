project("app")
kind("ConsoleApp")
language("C++")
cppdialect("C++20")
targetdir("binaries/%{cfg.buildcfg}")
staticruntime("off")

files({ "source/**.h", "source/**.cpp" })

includedirs({
	"source",

	-- Include Core
	"../core/source",
})

links({
	"core",
	"pqxx", -- libpqxx
	"pq", -- libpq (PostgreSQL)
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
