#pragma once

#include "engine/Core/Level.hpp"


namespace Rendering
{
	////////////////////////////////////////////////////////////
	class System
	{
		public:

			System() = default;
			~System() = default;

			void Render(sf::RenderWindow& _window);
			
			static unsigned int s_LevelTileSize;
			static sf::Vector2f s_RenderSize;
			static bool s_RenderEmptyTiles;
	};
}
