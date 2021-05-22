project "imguiFD"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "On"

	files
	{
		"*.h",
        "*.cpp",
        "dirent/dirent.h"
	}

	sysincludedirs
	{
        "../imgui"
	}

	filter "system:windows"
		systemversion "latest"

	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		runtime "Release"
		optimize "on"

	filter "configurations:Production"
		runtime "Release"
		optimize "on"


	