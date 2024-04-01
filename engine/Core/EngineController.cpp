#include "engine/pch.hpp"

#include "EngineController.hpp"
#include "engine/Filesystem/NativeFileSystem.hpp"
#include "engine/Filesystem/BinaryFilesystem.hpp"
#include "engine/Filesystem/FilesystemMark.hpp"
#include "engine/core/Level.hpp"
#include <iostream>

void EngineController::Update(float _dt)
{
	if (m_ScenesStateMachine)
	{
		m_ScenesStateMachine->Update(_dt);
	}

	if (m_GameObjectCollection)
	{
		m_GameObjectCollection->Update(_dt);
		m_GameObjectCollection->LateUpdate(_dt);
	}

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
		Assets::Storage* assets_storage = main_instance.GetEngineController().GetAssetsStorage();

		std::cout << "EngineModule: Finished filling assets storage.\n"
			<< "No. textures: " << assets_storage->GetTexturesCount() << "\n"
			<< "No. fonts: " << assets_storage->GetFontsCount() << "\n";
		m_ProjectResourcesInitFinished = false;
		OnAssetsStorageLoaded();
	}
}

std::string EngineController::GetEngineVersionString()
{
	const std::array<unsigned char, 4> engine_version_bytes = GetEngineVersion();

	// Convert the version array to a string.
	return "TODO";
}

void EngineController::OnProjectChanged()
{
	InitializeFilesystems();
}

void EngineController::OnFilesystemsLoaded()
{
	InitializeAssets();
}

void EngineController::OnAssetsStorageLoaded()
{
	auto& main_instance = ApplicationSingleton::Instance(); 
	main_instance.GetEngineController().GetAssetsStorage()->SetInitialized();
	main_instance.GetEngineController().GetAssetsStorage()->GetTilemapStorage()->InitImageBuffers();
}

void EngineController::InitializeFilesystems()
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
	game_resources_fs->SetProjectFilesystem(true);
	std::unique_ptr<fs::IFileSystem> game_data_fs = std::make_unique<fs::BinaryFileSystem>(data_path);
	game_data_fs->SetProjectFilesystem(true);

	sf::Mutex mutex;

	auto thread_init_fs = [&](const std::string& _fs_alias, std::unique_ptr<fs::IFileSystem>& _fs)
	{
		_fs->Initialize();

		mutex.lock();
		auto& main_instance = ApplicationSingleton::Instance();
		main_instance.GetEngineController().GetFilesystemManager()->Mount(_fs_alias, std::move(_fs));
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

void EngineController::InitializeAssets()
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
	fs::IFileSystem* resource_fs = main_instance.GetEngineController().GetFilesystemManager()->Get("resources");
	const std::vector<std::string> resources_file_aliases = resource_fs->GetFilesAliases();

	std::vector<fs::IFile*> textures_files;
	std::vector<fs::IFile*> fonts_files;
	std::vector<fs::IFile*> json_files;
	std::vector<fs::IFile*> binary_files;

	std::ranges::for_each(resources_file_aliases.cbegin(),
		resources_file_aliases.cend(),
		[&]
		(auto&& alias)
		{

			std::shared_ptr<fs::IFile> file = resource_fs->OpenFile(alias, fs::io::OpenMode::In | fs::io::OpenMode::Binary);

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
							json_files.push_back(file.get());
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

	fs::IFileSystem* data_fs = main_instance.GetEngineController().GetFilesystemManager()->Get("data");
	const std::vector<std::string> data_file_aliases = data_fs->GetFilesAliases();

	std::ranges::for_each(data_file_aliases.cbegin(),
				data_file_aliases.cend(),
		[&]
		(auto&& alias)
		{
			std::shared_ptr<fs::IFile> file = data_fs->OpenFile(alias, fs::io::OpenMode::In | fs::io::OpenMode::Binary);

			if (file)
			{
				switch (file->GetType())
				{
					case fs::IFile::EType::Data_Tilemaps:
					{
						Assets::TilemapStorage* tilemap_storage = main_instance.GetEngineController().GetAssetsStorage()->GetTilemapStorage();

						auto& tilemap_info = tilemap_storage->GetTilesetsInfo();
						auto binary_data = std::dynamic_pointer_cast<ISerializable::Binary>(tilemap_info);

						file->Seek(0, fs::io::Origin::Begin);
						file->DeserializeBinary(binary_data);
						tilemap_info = std::dynamic_pointer_cast<Serializable::Binary::TilesetsInfo>(binary_data);
						binary_files.push_back(file.get());
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
				std::cout << "ERROR: File with alias: " << alias << " could not be found in data fs.\n";
			}
		});

	Assets::Storage* assets_storage = main_instance.GetEngineController().GetAssetsStorage();
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
			f_close_files(json_files);
			f_close_files(binary_files);

			m_ProjectResourcesInitStarted = false;
			m_ProjectResourcesInitFinished = true;
			mutex.unlock();
	};

	sf::Thread thread_load_resources(load_resources_parallel);

	thread_load_resources.launch();
}

void EngineController::LoadLevelData(const Serializable::JSON::LevelInfo& _lvl_info)
{

}

