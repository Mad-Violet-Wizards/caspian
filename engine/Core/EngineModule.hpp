#pragma once

#include "engine/Tools/Tools.hpp"
#include "engine/Filesystem/FsManager.hpp"
#include "EventHandler.hpp"

class EngineModule
{

	public:

		EngineModule() = default;
		~EngineModule() = default;

		void SetEventDispatcher(std::unique_ptr<Events::Dispatcher> _event_dispatcher) { m_eventDispatcher = std::move(_event_dispatcher); }
		Events::Dispatcher* const GetEventDispatcher() { return m_eventDispatcher.get(); }

		void SetToolsManager(std::unique_ptr<Tools::Manager> _tools_mgr) { m_toolsManager = std::move(_tools_mgr); }
		Tools::Manager* const GetToolsManager() { return m_toolsManager.get(); }

		void SetFilesystemManager(std::unique_ptr<fs::Manager> _fs_mgr) { m_filesystemManager = std::move(_fs_mgr); }
		fs::Manager* const GetFilesystemManager() { return m_filesystemManager.get(); }

		static std::array<unsigned char, 4> GetEngineVersion() { return { 0x31, 0x30, 0x30, 0x30}; }

	private:

		std::unique_ptr<Events::Dispatcher> m_eventDispatcher = nullptr;
		std::unique_ptr<Tools::Manager> m_toolsManager = nullptr;
		std::unique_ptr<fs::Manager> m_filesystemManager = nullptr;

};