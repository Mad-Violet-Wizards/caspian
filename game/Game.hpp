#pragma once

#include "engine/Core/Window.hpp"

class Game
{
	public:

		Game();
		~Game();

		void GameLoop();

		void Update();
		void LateUpdate();
		void Draw();
		void CalculateDeltaTime();
		bool IsRunning() const;

	private:

		Window m_window;

		sf::Clock m_clock;
		float m_deltaTime;
};