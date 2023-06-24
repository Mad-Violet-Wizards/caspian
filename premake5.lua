workspace "Caspian"
  configurations { "Debug", "Release" }

project "GameEngine"
  kind "StaticLib"
  language "C++"
  targetdir "bin/%{cfg.buildcfg}"

  files { "engine/**.hpp", "engine/**.cpp" }

  includedirs { "vendor/include/" }
  libdirs { "vendor/lib/" }

  filter "configurations:Debug"
    defines { "DEBUG" }
    symbols "On"
    links { "sfml-graphics-d", "sfml-window-d", "sfml-system-d" }

  filter "configurations:Release"
    defines { "RELEASE" }
    optimize "On"
    links { "sfml-graphics", "sfml-window", "sfml-system" }

project "GameEngineTests"
  kind "ConsoleApp"
  language "C++"
  targetdir "bin/%{cfg.buildcfg}"

  files { "tests/**.hpp", "tests/**.cpp" }

  includedirs { "vendor/include/" }
  libdirs { "vendor/lib/" }

  filter "configurations:Debug"
    defines { "DEBUG" }
    symbols "On"
    links { "sfml-graphics-d", "sfml-window-d", "sfml-system-d", "GameEngine" }

  filter "configurations:Release"
    defines { "RELEASE" }
    optimize "On"
    links { "sfml-graphics", "sfml-window", "sfml-system", "GameEngine" }

project "Game"
  kind "ConsoleApp"
  language "C++"
  targetdir "bin/%{cfg.buildcfg}"

  files { "game/**.hpp", "game/**.cpp" }

  includedirs { "vendor/include/" }
  libdirs { "vendor/lib/" }

  filter "configurations:Debug"
    defines { "DEBUG" }
    symbols "On"
    links { "sfml-graphics-d", "sfml-window-d", "sfml-system-d", "GameEngine" }

  filter "configurations:Release"
    defines { "RELEASE" }
    optimize "On"
    links { "sfml-graphics", "sfml-window", "sfml-system", "GameEngine" }