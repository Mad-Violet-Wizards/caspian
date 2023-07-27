#pragma once

#include "engine/Core/Window.hpp"
#include "engine/Design-Patterns/Singleton.hpp"

namespace Game
{

	////////////////////////////////////////////////////////////////////////////////
	#if defined(DEBUG)
	class DebugHelper
	{
		public:

			DebugHelper() = default;
			~DebugHelper() = default;

			void InitializeDebugEventListeners();

		private:

			std::unique_ptr<EventListener> m_keyReleasedListener = nullptr;
	};
	#endif

	////////////////////////////////////////////////////////////////////////////////
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

		void SetEventDispatcher(std::unique_ptr<EventDispatcher> _event_dispatcher) { m_eventDispatcher = std::move(_event_dispatcher); }
		EventDispatcher* const GetEventDispatcher() { return m_eventDispatcher.get(); }

	private:

		Window m_window;

		std::unique_ptr<EventDispatcher> m_eventDispatcher;

		sf::Clock m_clock;
		float m_deltaTime;
	};

	////////////////////////////////////////////////////////////////////////////////
	using MainSingleton = Singleton<Main>;
};