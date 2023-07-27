#pragma once

#include "engine/Core/Window.hpp"
#include "engine/Design-Patterns/Singleton.hpp"

namespace Game
{
	class Main
	{
	public:

		Main();
		~Main();

		void MainLoop();

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

	using MainSingleton = Singleton<Main>;
};