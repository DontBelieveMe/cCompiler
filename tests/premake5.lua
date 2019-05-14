project "cCompiler-tests"
	kind "ConsoleApp"
	language "C++"

	targetdir "build/bin/%{cfg.buildcfg}"
	objdir "build/obj/%{cfg.buildcfg}"

	files { "**.cpp" }

	-- This is not an actual perf test, just a dump of a QuickBench session
	removefiles {
		"perf_stringcompareignorecase.cpp"
	}

	includedirs {
		"../vendor/spdlog/include",
		"../vendor/cxxopts",
		"../vendor/catch2",
		"../include"
	}

	links { "cCompiler" }

	filter "configurations:Debug"
		defines { "DEBUG" }
		symbols "On"

	filter "configurations:Release"
		defines { "NDEBUG" }
		optimize "On"
	
	filter "system:windows"
		systemversion "latest"
