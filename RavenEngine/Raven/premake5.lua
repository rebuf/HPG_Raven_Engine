



project "RavenEngine"
	kind "StaticLib"
	language "C++"
	editandcontinue "Off"

	if Arch == "x64" then
		postbuildcommands { 
			"powershell Copy-Item '"..root_dir.."/RavenEngine/Dependencies/glew-2.1.0/bin/Release/x64/glew32.dll' '"..root_dir.."/gameProject/'",
			"powershell Copy-Item '"..root_dir.."/RavenEngine/Dependencies/OpenAL/libs/Win32/OpenAL32.dll' '"..root_dir.."/gameProject/'" 
		}
	else
		postbuildcommands { "powershell Copy-Item '"..root_dir.."/RavenEngine/Dependencies/glew-2.1.0/bin/Release/win32/glew32.dll' '"..root_dir.."/gameProject/'"}
	end
	
	postbuildmessage "Copying dependencies..."

	files
	{
		"Source/**.h",
		"Source/**.c",
		"Source/**.cpp",
		"../../gameProject/scripts/**.lua",
		"../../gameProject/shaders/**.glsl"
	}

	removefiles
	{
		--"Source/Platform/**"
	}

	includedirs
	{
		"Source/",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.Glew}",
		"%{IncludeDir.stb}",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.Dependencies}",
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
		"%{IncludeDir.Raven}",
		"%{IncludeDir.OpenFBX}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.OpenAL}",
		"%{IncludeDir.reactphysics3d}",
		"%{IncludeDir.LuaBridge}",
		"%{IncludeDir.lua}",
		"%{IncludeDir.NodeEditor}",
		"%{IncludeDir.ImGuiFileDialog}"
	}

	links
	{
		"imgui",
		"spdlog",
		"imguiFD"
	}

	defines
	{
		"SPDLOG_COMPILED_LIB"
	}

	filter 'architecture:x86_64'
		defines { "RAVEN_SSE"}

	filter "system:windows"
		cppdialect "C++17"
		staticruntime "on"
		systemversion "latest"
		disablewarnings { 4307 }
		characterset ("MBCS")

		--pchheader "Precompiled.h"
		--pchsource "Source/Precompiled.cpp"

		defines
		{
			"_CRT_SECURE_NO_WARNINGS",
			"_DISABLE_EXTENDED_ALIGNED_STORAGE",
			"_SILENCE_CXX17_OLD_ALLOCATOR_MEMBERS_DEPRECATION_WARNING",
			"_SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING"
		}

		files
		{
		
		}

		if Arch == "x64" then
			libdirs 
			{
				"../Dependencies/glew-2.1.0/lib/Release/x64/",
				"../Dependencies/OpenAL/libs/Win32"
			}
		else
			libdirs 
			{
				"../Dependencies/glew-2.1.0/lib/Release/Win32/"
				
			}
		end
	

		links
		{
			"glfw","glew32","lua", "reactphysics3d", "openfbx","node-editor","OpenAL32"
		}

		buildoptions
		{
			"/MP", "/bigobj","/openmp"
		}

		filter 'files:External/**.cpp'
			flags  { 'NoPCH' }
		filter 'files:External/**.c'
			flags  { 'NoPCH' }


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
