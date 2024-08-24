project "ClientExample"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++20"
	staticruntime "off"

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"src/**.h",
		"src/**.cpp",
	}

	includedirs
	{
		"src",
		"../../AlmaasHttp/src",
	}

	links
	{
		"AlmaasHttp"
	}

	filter "system:windows"
		systemversion "latest"

		defines
		{
			"ALMAAS_PLATFORM_WINDOWS"
		}

	filter "configurations:Debug"
		defines "ALMAAS_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "ALMAAS_RELEASE"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		defines "ALMAAS_DIST"
		runtime "Release"
		optimize "on"
