workspace "cCompiler"
	configurations { "Debug", "Release" }

include "tests"

project "cCompiler"
	kind "StaticLib"
	language "C++"

	targetdir "build/bin/%{cfg.buildcfg}"
	objdir "build/obj/%{cfg.buildcfg}"
	
	os.execute("python tools/x86-gen.py")
	os.copyfile("tools/x86.cpp", "src/x86.cpp")
	os.copyfile("tools/x86.h", "include/cc/x86.h")

	files { "include/**.h", "src/**.cpp" }
	removefiles { "src/main.cpp" }
	
	includedirs { 
		"include",
		"vendor/spdlog/include",
		"vendor/cxxopts"
	}

	filter "configurations:Debug"
		defines { "DEBUG" }
		symbols "On"
	
	filter "configurations:Release"
		defines { "NDEBUG" }
		optimize "On"

	filter "system:windows"
		systemversion "latest"

project "cCompiler-as"
	kind "ConsoleApp"
	language "C++"

	targetdir "build/bin/%{cfg.buildcfg}"
	objdir "build/obj/%{cfg.buildcfg}"

	files { "src/main.cpp" }

	includedirs {
		"include",
		"vendor/spdlog/include",
		"vendor/cxxopts"
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

