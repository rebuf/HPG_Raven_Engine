require 'Scripts/premake-defines'
require 'Scripts/premake-common'
require 'Scripts/premake-triggers'
require 'Scripts/premake-settings'

root_dir = os.getcwd()

Arch = _OPTIONS["arch"]

IncludeDir = {}
IncludeDir["GLFW"] = "%{root_dir}/RavenEngine/Dependencies/glfw/include/"
IncludeDir["Glew"] = "%{root_dir}/RavenEngine/Dependencies/glew-2.1.0/include/"
IncludeDir["stb"] = "%{root_dir}/RavenEngine/Dependencies/stb/"
IncludeDir["Raven"] = "%{root_dir}/RavenEngine/Raven/Source"
IncludeDir["Dependencies"] = "%{root_dir}/RavenEngine/Dependencies/"
IncludeDir["ImGui"] = "%{root_dir}/RavenEngine/Dependencies/imgui/"
IncludeDir["cereal"] = "%{root_dir}/RavenEngine/Dependencies/cereal/include"
IncludeDir["spdlog"] = "%{root_dir}/RavenEngine/Dependencies/spdlog/include"
IncludeDir["glm"] = "%{root_dir}/RavenEngine/Dependencies/glm"
IncludeDir["OpenFBX"] ="%{root_dir}/RavenEngine/Dependencies/OpenFBX/"
IncludeDir["reactphysics3d"] = "%{root_dir}/RavenEngine/Dependencies/rp3d/include"
IncludeDir["lua"] ="%{root_dir}/RavenEngine/Dependencies/lua/src"
IncludeDir["LuaBridge"] ="%{root_dir}/RavenEngine/Dependencies/LuaBridge"
IncludeDir["NodeEditor"] = "%{root_dir}/RavenEngine/Dependencies/node-editor"
IncludeDir["ImGuiFileDialog"] = "%{root_dir}/RavenEngine/Dependencies/ImGuiFD"
IncludeDir["OpenAL"] = "%{root_dir}/RavenEngine/Dependencies/OpenAL/include/"


workspace( settings.workspace_name )
	location "build"
	flags { 'MultiProcessorCompile' }
	
	outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
	targetdir ("bin/%{outputdir}/")
	objdir ("bin-int/%{outputdir}/obj/")


	if Arch == "x64" then 
		architecture "x86_64"
	elseif Arch == "x86" then
		architecture "x86"
	end

	print("Arch = ", Arch)

	configurations
	{
		"Debug",
		"Release",
		"Production"
	}

	group "External"
	
		require("RavenEngine/Dependencies/imgui/premake5")
			SetRecommendedSettings()
		require("RavenEngine/Dependencies/spdlog/premake5")
			SetRecommendedSettings()
		require("RavenEngine/Dependencies/GLFWpremake5")
			SetRecommendedSettings()
		require("RavenEngine/Dependencies/OpenFBX/premake5")
			SetRecommendedSettings()
		require("RavenEngine/Dependencies/rp3d/premake5")
			SetRecommendedSettings()
		require("RavenEngine/Dependencies/lua/premake5")
			SetRecommendedSettings()
		require("RavenEngine/Dependencies/node-editor/premake5")
			SetRecommendedSettings()
		require("RavenEngine/Dependencies/ImGuiFD/premake5")
			SetRecommendedSettings()			

	filter {}
	group ""

	include "RavenEngine/Raven/premake5"
	include "RavenEngine/Game/premake5"
	include "RavenEngine/Editor/premake5"

workspace( settings.workspace_name )

	startproject("RavenEditor")
