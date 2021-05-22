

project "RavenEditor"
	kind "ConsoleApp"
	language "C++"
	editandcontinue "Off"
	debugdir (root_dir.."/gameProject/")
	
	files
	{
		"**.h",
		"**.cpp",
		"Source/**.h",
		"Source/**.cpp"
	}
	
	includedirs
	{
		"../RavenEngine/Source",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.Glew}",
		"%{IncludeDir.stb}",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.spdlog}",
		"%{IncludeDir.cereal}",
		"%{IncludeDir.Raven}",
		"%{IncludeDir.OpenFBX}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.reactphysics3d}",
		"%{IncludeDir.LuaBridge}",
		"%{IncludeDir.lua}",
		"%{IncludeDir.NodeEditor}",
		"%{IncludeDir.ImGuiFileDialog}",
		"%{IncludeDir.OpenAL}"
	}

	sysincludedirs
	{
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.Glew}",
		"%{IncludeDir.stb}",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.Dependencies}",
		"%{IncludeDir.spdlog}",
		"%{IncludeDir.cereal}",
		"%{IncludeDir.OpenAL}",
		"%{IncludeDir.Raven}",
		"%{IncludeDir.OpenFBX}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.reactphysics3d}",
		"%{IncludeDir.LuaBridge}",
		"%{IncludeDir.lua}",
		"%{IncludeDir.NodeEditor}",
		"%{IncludeDir.ImGuiFileDialog}"
	}

	links
	{
		"RavenEngine",
		"imgui",
		"spdlog",
		"imguiFD"
	}

	defines
	{
		"SPDLOG_COMPILED_LIB"
	}

	filter { "files:Dependencies/**"}
		warnings "Off"

	filter 'architecture:x86_64'
		defines { "RAVEN_SSE"}

	filter "system:windows"
		cppdialect "C++17"
		staticruntime "On"
		systemversion "latest"
		--entrypoint "WinMainCRTStartup"
		--entrypoint "mainCRTStartup"

		defines
		{

			"_CRT_SECURE_NO_WARNINGS",
			"_DISABLE_EXTENDED_ALIGNED_STORAGE",
			"_SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING",
		}

		libdirs
		{
			--"../RAVEN/Dependencies/libs" 
			"../Dependencies/OpenAL/libs/Win32"
		}

		links
		{
			"OpenAL32",
			"glfw",
			"OpenGL32",
			"lua",
			"openfbx",
			"node-editor",
			"imguiFD"	
		}

		disablewarnings { 4307 }

	filter "configurations:Debug"
		defines { "RAVEN_DEBUG", "_DEBUG","TRACY_ENABLE","RAVEN_PROFILE", }
		symbols "On"
		runtime "Debug"
		optimize "Off"

	filter "configurations:Release"
		defines { "RAVEN_RELEASE","TRACY_ENABLE", "RAVEN_PROFILE",}
		optimize "Speed"
		symbols "On"
		runtime "Release"

	filter "configurations:Production"
		defines "RAVEN_PRODUCTION"
		symbols "Off"
		optimize "Full"
		runtime "Release"
