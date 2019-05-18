workspace "cCompiler"
	configurations { "Debug", "Release" }

include "tests"

project "cCompiler"
	kind "StaticLib"
	language "C++"

	targetdir "build/bin/%{cfg.buildcfg}"
	objdir "build/obj/%{cfg.buildcfg}"

	print("Generating source files...")
	os.execute("python tools/x86-gen.py")

	print("Copying generated files to source tree...")
	os.copyfile("tools/x86_data.cpp", "src/x86_data.cpp")
	os.copyfile("tools/x86_data.h", "include/cc/x86_data.h")

	files { "include/**.h", "src/**.cpp", "tools/*.py" }
	removefiles {
		"src/main.cpp"
	}
	
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

