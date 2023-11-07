#pragma once

#include "engine/Tools/Tools.hpp"
#include "engine/Filesystem/FsManager.hpp"
#include "EventHandler.hpp"
#include "Assets.hpp"

////////////////////////////////////////////////////////////////////////////////
struct Project
{
	std::string m_ProjectName;
	std::string m_ProjectPath;

	[[nodiscard]] bool operator==(const Project& _other) const
	{
		return m_ProjectName == _other.m_ProjectName && 
					 m_ProjectPath == _other.m_ProjectPath;
	}
};

////////////////////////////////////////////////////////////////////////////////
class EngineModule
{

	public:

		EngineModule() = default;
		~EngineModule() = default;

		void Update();

		void SetEventDispatcher(std::unique_ptr<Events::Dispatcher> _event_dispatcher) { m_eventDispatcher = std::move(_event_dispatcher); }
		Events::Dispatcher* const GetEventDispatcher() { return m_eventDispatcher.get(); }

		void SetToolsManager(std::unique_ptr<Tools::Manager> _tools_mgr) { m_toolsManager = std::move(_tools_mgr); }
		Tools::Manager* const GetToolsManager() { return m_toolsManager.get(); }

		void SetFilesystemManager(std::unique_ptr<fs::Manager> _fs_mgr) { m_filesystemManager = std::move(_fs_mgr); }
		fs::Manager* const GetFilesystemManager() { return m_filesystemManager.get(); }

		void SetAssetsStorage(std::unique_ptr<Assets::Storage> _assets_storage) { m_assetsStorage = std::move(_assets_storage); }
		Assets::Storage* const GetAssetsStorage() { return m_assetsStorage.get(); }

		static std::array<unsigned char, 4> GetEngineVersion() { return { 0x31, 0x30, 0x30, 0x30}; }
		static std::string GetEngineVersionString();

		void SetCurrentProject(const Project& _project) { m_CurrentProject = _project; OnProjectChanged(); }
		const Project& GetCurrentProject() const { return m_CurrentProject.value(); }
		bool IsAnyProjectLoaded() const { return m_CurrentProject.has_value(); }
		void UnloadCurrentProject() { m_CurrentProject.reset(); }

	private:

		void OnProjectChanged();
		void OnFilesystemsLoaded();
		void OnAssetsStorageLoaded();

		void InitializeFilesystems();
		void InitializeAssets();

	private:

		std::unique_ptr<Events::Dispatcher> m_eventDispatcher = nullptr;
		std::unique_ptr<Tools::Manager> m_toolsManager = nullptr;
		std::unique_ptr<fs::Manager> m_filesystemManager = nullptr;
		std::unique_ptr<Assets::Storage> m_assetsStorage = nullptr;

		std::optional<Project> m_CurrentProject = std::nullopt;

		std::atomic<bool> m_ResourcesFsInitStarted = false;
		std::atomic<bool> m_DataFsInitStarted = false;
		std::atomic<bool> m_ProjectResourcesInitStarted = false;

		std::atomic<bool> m_ResourcesFsInitFinished = false;
		std::atomic<bool> m_DataFsInitFinished = false;
		std::atomic<bool> m_ProjectResourcesInitFinished = false;

};