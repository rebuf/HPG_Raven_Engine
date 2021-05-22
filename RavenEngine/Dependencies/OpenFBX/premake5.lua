
project "OpenFBX"
kind "StaticLib"
language "C++"
staticruntime "On"

files
{
  "miniz.c",
  "miniz.h",
  "ofbx.cpp",
  "ofbx.h"
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


  
