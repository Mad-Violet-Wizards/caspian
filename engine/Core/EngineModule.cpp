#include "engine/pch.hpp"

#include "EngineModule.hpp"
#include "engine/Filesystem/NativeFileSystem.hpp"
#include "engine/Filesystem/BinaryFilesystem.hpp"
#include "engine/Filesystem/FilesystemMark.hpp"
#include "engine/core/Level.hpp"
#include <iostream>

void EngineModule::Update()
{
	if (m_ResourcesFsInitFinished && m_DataFsInitFinished)
	{
		m_toolsManager->ShowNotification(Tools_Impl::ENotificationType::Success, "FS: Filesystems initialized.\nResources & data.");
		m_ResourcesFsInitFinished = false;
		m_DataFsInitFinished = false;
		OnFilesystemsLoaded();
	}

	if (m_ProjectResourcesInitFinished)
	{
		auto& main_instance = ApplicationSingleton::Instance();
		Assets::Storage* assets_storage = main_instance.GetAssetsStorage();

		std::cout << "EngineModule: Finished filling assets storage.\n"
			<< "No. textures: " << assets_storage->GetTexturesCount() << "\n"
			<< "No. fonts: " << assets_storage->GetFontsCount() << "\n";
		m_ProjectResourcesInitFinished = false;
		OnAssetsStorageLoaded();
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

void EngineModule::OnFilesystemsLoaded()
{
	InitializeAssets();
}

void EngineModule::OnAssetsStorageLoaded()
{
	auto& main_instance = ApplicationSingleton::Instance(); 
	main_instance.GetAssetsStorage()->SetInitialized();
}

void EngineModule::InitializeFilesystems()
{
	if (m_ResourcesFsInitStarted || m_DataFsInitStarted)
		return;

	Projects::Manager* project_manager = ApplicationSingleton::Instance().GetProjectsManager();

	if (!project_manager->IsAnyProjectLoaded())
		return;

	const std::string project_path = project_manager->GetCurrentProject().m_ProjectPath + "\\" + project_manager->GetCurrentProject().m_ProjectName;

	const std::string resources_path = project_path + "\\resources";
	const std::string data_path = project_path + "\\data";

	std::unique_ptr<fs::IFileSystem> game_resources_fs = std::make_unique<fs::NativeFileSystem>(resources_path);
	std::unique_ptr<fs::IFileSystem> game_data_fs = std::make_unique<fs::BinaryFileSystem>(data_path);

	sf::Mutex mutex;

	auto thread_init_fs = [&](const std::string& _fs_alias, std::unique_ptr<fs::IFileSystem>& _fs)
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

void EngineModule::InitializeAssets()
{
	auto fn_contains = [](const std::string_view _str1, const std::string_view _str2) -> bool
	{
			if (_str1.size() < _str2.size())
				return false;

			if (_str1.find(_str2) != std::string_view::npos)
				return true;

			return false;
	};

	auto& main_instance = ApplicationSingleton::Instance();
	fs::IFileSystem* resource_fs = main_instance.GetFilesystemManager()->Get("resources");
	const std::vector<std::string> file_aliases = resource_fs->GetFilesAliases();

	std::vector<fs::IFile*> textures_files;
	std::vector<fs::IFile*> fonts_files;

	std::ranges::for_each(file_aliases.cbegin(),
		file_aliases.cend(),
		[&]
		(auto&& alias)
		{

			std::shared_ptr<fs::IFile> file = resource_fs->OpenFile(alias, fs::io::OpenMode::In);

			if (file)
			{
				switch (file->GetType())
				{
					case fs::IFile::EType::Texture:
					{
						textures_files.push_back(file.get());
						break;
					}
					case fs::IFile::EType::Font:
					{
						fonts_files.push_back(file.get());
						break;
					}
					case fs::IFile::EType::JSON:
					{
						if (fn_contains(alias, "level"))
						{
							std::shared_ptr<ISerializable::JSON> json_load_wrapper = std::make_shared<Serializable::JSON::LevelInfo>();

							file->Seek(0, fs::io::Origin::Begin);
							file->DeserializeJson(json_load_wrapper);

							auto level_info { std::dynamic_pointer_cast<Serializable::JSON::LevelInfo>(json_load_wrapper) };
							main_instance.GetWorld()->PushInitialLevelData(level_info);
						}
							break;
					}
					default:
					{
						std::cout << "WARNING: File with alias: " << alias << " has an invalid type and couldn't be assigned to any pool.\n";
						break;
					}
				}
			}
			else
			{
				std::cout << "ERROR: File with alias: " << alias << " could not be found in resources fs.\n";
			}
		});

	Assets::Storage* assets_storage = main_instance.GetAssetsStorage();
	sf::Mutex mutex;

	auto f_close_files = [](std::vector<fs::IFile*> _files)
	{
			for (auto f : _files)
				f->Close();
	};

	auto load_resources_parallel = [&]()
	{
			mutex.lock();
			m_ProjectResourcesInitStarted = true;
			mutex.unlock();

			assets_storage->LoadTextureFsFilesBatch(textures_files);
			assets_storage->LoadFontFsFilesBatch(fonts_files);

			mutex.lock();
			f_close_files(textures_files);
			f_close_files(fonts_files);

			m_ProjectResourcesInitStarted = false;
			m_ProjectResourcesInitFinished = true;
			mutex.unlock();
	};

	sf::Thread thread_load_resources(load_resources_parallel);

	thread_load_resources.launch();
}

