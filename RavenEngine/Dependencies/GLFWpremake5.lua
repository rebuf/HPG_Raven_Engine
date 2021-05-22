project "glfw"
  kind "StaticLib"
  language "C"
  systemversion "latest"

  removeplatforms "emscripten"

  filter "configurations:Debug"
		symbols "On"

  filter "configurations:Release"
    runtime "Release"
		optimize "On"

  filter "configurations:Production"
    runtime "Release"
    optimize "On"

  filter {}

  files {
    "glfw/src/context.c",
    "glfw/src/init.c",
    "glfw/src/input.c",
    "glfw/src/monitor.c",
    "glfw/src/vulkan.c",
    "glfw/src/window.c",
    "glfw/src/glfw_config.h",
    "glfw/include/GLFW/glfw3.h",
    "glfw/include/GLFW/glfw3native.h"
  }

  includedirs {
    "glfw/include"
  }

  
  filter "system:win32 or win64 or mingw32"
    defines {
      "_GLFW_WIN32",
      "_CRT_SECURE_NO_WARNINGS"
    }

	staticruntime "On"

   	buildoptions { "/MP" }

    files {
      "glfw/src/win32_platform.h",
      "glfw/src/win32_joystick.h",
      "glfw/src/wgl_context.h",
      "glfw/src/egl_context.h",
      "glfw/src/osmesa_context.h",
      "glfw/src/win32_init.c",
      "glfw/src/win32_joystick.c",
      "glfw/src/win32_monitor.c",
      "glfw/src/win32_time.c",
      "glfw/src/win32_window.c",
      "glfw/src/win32_thread.c",
      "glfw/src/wgl_context.c",
      "glfw/src/egl_context.c",
      "glfw/src/osmesa_context.c",
	  "stb/stb_vorbis.c"--write in here, because I'm lazy to write another premake file
    }


    buildoptions
    {
      "-fPIC"
    }
