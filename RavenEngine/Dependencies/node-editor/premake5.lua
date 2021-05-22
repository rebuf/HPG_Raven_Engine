project "node-editor"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "On"

	files
	{
		"crude_json.cpp",
		"crude_json.h",
		"imgui_bezier_math.h",
		"imgui_bezier_math.inl",
		"imgui_canvas.cpp",
		"imgui_canvas.h",
		"imgui_extra_math.h",
		"imgui_extra_math.inl",
		"imgui_node_editor.cpp",
		"imgui_node_editor.h",
		"imgui_node_editor_api.cpp",
		"imgui_node_editor_internal.h",
		"imgui_node_editor_internal.inl",
	}

	sysincludedirs
	{
		"../imgui/"
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


	