#pragma once

#include "engine/Core/Window.hpp"
#include "engine/Design-Patterns/Singleton.hpp"
#include "engine/core/EngineModule.hpp"
#include "engine/Core/Level.hpp"

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

		void SetEventDispatcher(std::unique_ptr<Events::Dispatcher> _event_dispatcher) { m_engineModule.SetEventDispatcher(std::move(_event_dispatcher)); }
		Events::Dispatcher* const GetEventDispatcher() { return m_engineModule.GetEventDispatcher(); }

		void SetToolsManager(std::unique_ptr<Tools::Manager> _tools_mgr) { m_engineModule.SetToolsManager(std::move(_tools_mgr)); }
		Tools::Manager* const GetToolsManager() { return m_engineModule.GetToolsManager(); }

		void SetFilesystemManager(std::unique_ptr<fs::Manager> _fs_mgr) { m_engineModule.SetFilesystemManager(std::move(_fs_mgr)); }
		fs::Manager* const GetFilesystemManager() { return m_engineModule.GetFilesystemManager(); }

		void SetAssetsStorage(std::unique_ptr<Assets::Storage> _assets_storage) { m_engineModule.SetAssetsStorage(std::move(_assets_storage)); }
		Assets::Storage* const GetAssetsStorage() { return m_engineModule.GetAssetsStorage(); }

		void SetWorld(std::unique_ptr<Level::World> _world) { m_World = std::move(_world); }
		Level::World* const GetWorld() { return m_World.get(); }

		EngineModule& GetEngineModule() { return m_engineModule; }

		void UpdateWindowTitle(const std::string& _title) { m_window.UpdateTitle(_title); }

	private:

		Window m_window;

		EngineModule m_engineModule;

		std::unique_ptr<Level::World> m_World = nullptr;

		sf::Clock m_clock;
		float m_deltaTime;
	};

namespace Windows
{
	static std::string S_ENGINE_APPDATA_ALIAS = "/appdata/caspian/engine/";
}

////////////////////////////////////////////////////////////////////////////////
using ApplicationSingleton = Singleton<Application>;