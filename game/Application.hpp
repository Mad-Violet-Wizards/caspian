#pragma once

#include "engine/Core/Window.hpp"
#include "engine/Design-Patterns/Singleton.hpp"
#include "engine/core/EngineModule.hpp"
#include "engine/Core/Level.hpp"
#include "engine/Core/Projects.hpp"

////////////////////////////////////////////////////////////////////////////////
#if defined(DEBUG)
class DebugHelper
{
	public:

		DebugHelper() = default;
		~DebugHelper() = default;

		void InitializeDebugEventListeners();

	private:

		std::unique_ptr<Events::Listener> m_keyReleasedListener = nullptr;
	};
	#endif

	////////////////////////////////////////////////////////////////////////////////
	class Application
	{
	public:

		Application();
		~Application();

		void MainLoop();

		void Update();
		void LateUpdate();
		void Draw();
		void CalculateDeltaTime();
		bool IsRunning() const;

		EngineModule& GetEngineModule() { return m_engineModule; }

		void SetWorld(std::unique_ptr<Level::World> _world) { m_World = std::move(_world); }
		Level::World* const GetWorld() { return m_World.get(); }

		void SetProjectsManager(std::unique_ptr<Projects::Manager> _pm) { m_ProjectsManager = std::move(_pm); }
		Projects::Manager* const GetProjectsManager() { return m_ProjectsManager.get(); }

		void UpdateWindowTitle(const std::string& _title) { m_window.UpdateTitle(_title); }

	private:

		Window m_window;

		EngineModule m_engineModule;

		std::unique_ptr<Level::World> m_World = nullptr;
		std::unique_ptr<Projects::Manager> m_ProjectsManager;

		sf::Clock m_clock;
		float m_deltaTime;
	};

namespace Windows
{
	static std::string S_ENGINE_APPDATA_ALIAS = "/appdata/caspian/engine/";
}

////////////////////////////////////////////////////////////////////////////////
using ApplicationSingleton = Singleton<Application>;