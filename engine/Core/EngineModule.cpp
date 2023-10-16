#include "engine/pch.hpp"

#include "EngineModule.hpp"
#include "engine/Filesystem/NativeFileSystem.hpp"
#include "game/Application.hpp"
#include <iostream>

void EngineModule::Update()
{
	if (m_ResourcesFsInitFinished && m_DataFsInitFinished)
	{
		m_toolsManager->ShowNotification(Tools_Impl::ENotificationType::Success, "FS: Filesystems initialized.\nResources & data.");
		m_ResourcesFsInitFinished = false;
		m_DataFsInitFinished = false;
	}
}

std::string EngineModule::GetEngineVersionString()
{
	const std::array<unsigned char, 4> engine_version_bytes = GetEngineVersion();

	// Convert the version array to a string.
	return "TODO";
}

void EngineModule::OnProjectChanged()
{
	InitializeFilesystems();
}

void EngineModule::InitializeFilesystems()
{
	if (m_ResourcesFsInitStarted || m_DataFsInitStarted)
		return;

	if (m_CurrentProject == std::nullopt)
		return;

	const std::string project_path = m_CurrentProject->m_ProjectPath + "\\" + m_CurrentProject->m_ProjectName;

	const std::string resources_path = project_path + "\\resources";
	const std::string data_path = project_path + "\\data";

	auto game_resources_fs = std::make_unique<fs::NativeFileSystem>(resources_path);
	auto game_data_fs = std::make_unique<fs::NativeFileSystem>(data_path);

	sf::Mutex mutex;

	auto thread_init_fs = [&](const std::string& _fs_alias, std::unique_ptr<fs::NativeFileSystem>& _fs)
	{
		_fs->Initialize();

		mutex.lock();
		auto& main_instance = ApplicationSingleton::Instance();
		main_instance.GetFilesystemManager()->Mount(_fs_alias, std::move(_fs));
		mutex.unlock();
	};
	
	auto init_res_fs = [&]() 
	{
		mutex.lock();
		m_ResourcesFsInitStarted = true;
		mutex.unlock();

		thread_init_fs("resources", game_resources_fs); 

		mutex.lock();
		m_ResourcesFsInitStarted = false;
		m_ResourcesFsInitFinished = true;
		mutex.unlock();
	};

	auto init_data_fs = [&]() 
	{
		mutex.lock();
		m_DataFsInitStarted = true;
		mutex.unlock();
		
		thread_init_fs("data", game_data_fs); 

		mutex.lock();
		m_DataFsInitStarted = false;
		m_DataFsInitFinished = true;
		mutex.unlock();
	};

	sf::Thread thread_resources_fs(init_res_fs);
	sf::Thread thread_data_fs(init_data_fs);

	thread_resources_fs.launch();
	thread_data_fs.launch();
}
