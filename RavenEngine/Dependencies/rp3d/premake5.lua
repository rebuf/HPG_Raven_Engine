project "reactphysics3d"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"
    staticruntime "On"

    rp3d_root_dir = os.getcwd()

    files {
        rp3d_root_dir .. "/include/**.h",
        rp3d_root_dir .. "/src/**.cpp"
    }

    sysincludedirs {
        rp3d_root_dir .. "/include/"
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


