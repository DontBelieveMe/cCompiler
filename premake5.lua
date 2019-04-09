workspace "cCompiler"
	configurations { "Debug", "Release" }

project "cCompiler"
	kind "ConsoleApp"
	language "C++"

	targetdir "build/bin/%{cfg.buildcfg}"
	objdir "build/obj/%{cfg.buildcfg}"

	files { "include/**.h", "src/**.cpp", "src/**.js", "src/**.cdef" }
	
	includedirs { 
		"include",
		"vendor/spdlog/include",
		"vendor/cxxopts"
	}

	os.execute("node src/x86/x86-gen.js")

	filter "configurations:Debug"
		defines { "DEBUG" }
		symbols "On"
	
	filter "configurations:Release"
		defines { "NDEBUG" }
		optimize "On"

	filter "system:windows"
		systemversion "latest"
