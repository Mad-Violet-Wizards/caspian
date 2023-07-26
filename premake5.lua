workspace "Caspian"
  configurations { "Debug", "Release" }

project "GameEngine"
  kind "StaticLib"
  language "C++"
  targetdir "build/%{cfg.buildcfg}"

  pchheader "engine/pch.hpp"
  pchsource "engine/pch.cpp"

  files { "engine/**.hpp", "engine/**.cpp" }

  filter "configurations:*"
    includedirs { "vendor/include/", "." }
    libdirs { "vendor/lib/" }

  filter "configurations:Debug"
    defines { "DEBUG" }
    symbols "On"
    links { "sfml-system-d", "sfml-window-d", "sfml-graphics-d", "sfml-network-d", "sfml-audio-d"}
    postbuildcommands { "{COPY} %{wks.location}/vendor/bin/*.dll %{cfg.targetdir}"}

  filter "configurations:Release"
    defines { "RELEASE" }
    optimize "On"
    links { "sfml-system", "sfml-window", "sfml-graphics", "sfml-network", "sfml-audio"}
    postbuildcommands { "{COPY} %{wks.location}/vendor/bin/*.dll %{cfg.targetdir}"}

project "GameEngineTests"
  kind "ConsoleApp"
  language "C++"
  targetdir "build/%{cfg.buildcfg}"

  files { "tests/**.hpp", "tests/**.cpp" }

  filter "configurations:*"
    includedirs { "vendor/include/", "." }
    libdirs { "vendor/lib/" }

  filter "configurations:Debug"
    defines { "DEBUG" }
    symbols "On"
    links { "GameEngine" }

  filter "configurations:Release"
    defines { "RELEASE" }
    optimize "On"
    links { "GameEngine" }

project "Game"
  kind "ConsoleApp"
  language "C++"
  targetdir "build/%{cfg.buildcfg}"

  pchheader "game/pch.hpp"
  pchsource "game/pch.cpp"

  files { "game/**.hpp", "game/**.cpp" }

  filter "configurations:*"
    includedirs { "vendor/include/", "." }
    libdirs { "vendor/lib/" }

  filter "configurations:Debug"
    defines { "DEBUG" }
    symbols "On"
    links { "GameEngine" }

  filter "configurations:Release"
    defines { "RELEASE" }
    optimize "On"
    links { "GameEngine" }