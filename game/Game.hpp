#pragma once

#include "engine/Core/Window.hpp"
#include "engine/Design-Patterns/Singleton.hpp"
#include "engine/Tools/Tools.hpp"
#include "engine/Filesystem/FsManager.hpp"

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

			std::unique_ptr<Events::Listener> m_keyReleasedListener = nullptr;
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

		void SetEventDispatcher(std::unique_ptr<Events::Dispatcher> _event_dispatcher) { m_eventDispatcher = std::move(_event_dispatcher); }
		Events::Dispatcher* const GetEventDispatcher() { return m_eventDispatcher.get(); }

		void SetToolsManager(std::unique_ptr<Tools::Manager> _tools_mgr) { m_toolsManager = std::move(_tools_mgr); }
		Tools::Manager* const GetToolsManager() { return m_toolsManager.get(); }

		void SetFilesystemManager(std::unique_ptr<fs::Manager> _fs_mgr) { m_filesystemManager = std::move(_fs_mgr); }
		fs::Manager* const GetFilesystemManager() { return m_filesystemManager.get(); }

	private:

		Window m_window;

		std::unique_ptr<Events::Dispatcher> m_eventDispatcher = nullptr;
		std::unique_ptr<Tools::Manager> m_toolsManager = nullptr;
		std::unique_ptr<fs::Manager> m_filesystemManager = nullptr;

		sf::Clock m_clock;
		float m_deltaTime;
	};

	namespace Windows
	{
		static std::string S_ENGINE_APPDATA_ALIAS = "/appdata/caspian/engine/";
	}


	////////////////////////////////////////////////////////////////////////////////
	using MainSingleton = Singleton<Main>;
};