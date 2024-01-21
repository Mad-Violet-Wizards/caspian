#pragma once

// SFML //

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <SFML/Network.hpp>
#include <SFML/Audio.hpp>

// STL //

#include <string>
#include <string_view>
#include <array>
#include <vector>
#include <map>
#include <unordered_map>
#include <set>
#include <memory>
#include <algorithm>
#include <filesystem>
#include <future>
#include <ranges>
#include <queue>
#include <cstdint> // https://en.cppreference.com/w/cpp/types/integer

// ENGINE //

#include "engine/Core/IComponent.hpp"
#include "engine/Core/EventHandler.hpp"
#include "engine/Core/GameObject.hpp"
#include "engine/Core/Random/UUID.hpp"
#include "game/Application.hpp"

// WARNING C4603.
#if(defined DEBUG)
	#if defined(_MSC_VER) && !defined(_CRT_SECURE_NO_WARNINGS)
		#define _CRT_SECURE_NO_WARNINGS
	#endif
#endif
