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
#include <vector>
#include <map>
#include <unordered_map>
#include <set>
#include <memory>
#include <algorithm>

// ENGINE //

#include "engine/Core/IComponent.hpp"
#include "engine/Core/EventHandler.hpp"
#include "engine/Core/GameObject.hpp"

// WARNING C4603.
#if(defined DEBUG)
	#if defined(_MSC_VER) && !defined(_CRT_SECURE_NO_WARNINGS)
		#define _CRT_SECURE_NO_WARNINGS
	#endif
#endif
