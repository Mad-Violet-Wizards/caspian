#pragma once

#include "engine/Core/Window.hpp"
#include "engine/Design-Patterns/Singleton.hpp"
#include "engine/core/EngineController.hpp"
#include "engine/Core/Level.hpp"
#include "engine/Core/Projects.hpp"
#include "engine/Core/Rendering.hpp"
#include "engine/Core/DebugEditorControllers.hpp"

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

		// Use for debug purposes only!
		float GetDeltaTime() const { return m_deltaTime; }

		EngineController& GetEngineController() { return m_engineController; }
		DebugEditorControllers& GetDebugControllers() { return m_debugControllers; }

		void SetWorld(std::unique_ptr<Level::World> _world) { m_World = std::move(_world); }
		Level::World* const GetWorld() { return m_World.get(); }

		void SetProjectsManager(std::unique_ptr<Projects::Manager> _pm) { m_ProjectsManager = std::move(_pm); }
		Projects::Manager* const GetProjectsManager() { return m_ProjectsManager.get(); }

		void SetRenderingSystem(std::unique_ptr<Rendering::System> _renderingSystem) { m_RenderingSystem = std::move(_renderingSystem); }
		Rendering::System* const GetRenderingSystem() { return m_RenderingSystem.get(); }

		void UpdateWindowTitle(const std::string& _title) { m_window.UpdateTitle(_title); }

		sf::Vector2i GetMousePosition();
		sf::Vector2f GetMousePositionWorld();

		const sf::View& GetView() const { return m_window.GetView(); }
			 
	private:

		Window m_window;

		EngineController m_engineController;
		DebugEditorControllers m_debugControllers;

		std::unique_ptr<Level::World> m_World = nullptr;
		std::unique_ptr<Projects::Manager> m_ProjectsManager;
		std::unique_ptr<Rendering::System> m_RenderingSystem;

		sf::Clock m_clock;
		float m_deltaTime;
	};

namespace Windows
{
	static std::string S_ENGINE_APPDATA_ALIAS = "/appdata/caspian/engine/";
}

////////////////////////////////////////////////////////////////////////////////
using ApplicationSingleton = Singleton<Application>;