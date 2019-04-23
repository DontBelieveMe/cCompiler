project "cCompiler-tests"
	kind "ConsoleApp"
	language "C++"

	targetdir "build/bin/%{cfg.buildcfg}"
	objdir "build/obj/%{cfg.buildcfg}"

	files { "**.cpp" }

	includedirs {
		"../vendor/spdlog/include",
		"../vendor/cxxopts",
		"../vendor/catch2"
	}

	filter "configurations:Debug"
		defines { "DEBUG" }
		symbols "On"

	filter "configurations:Release"
		defines { "NDEBUG" }
		optimize "On"
	
	filter "system:windows"
		systemversion "latest"
