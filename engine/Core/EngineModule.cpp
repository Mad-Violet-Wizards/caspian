#include "engine/pch.hpp"

#include "EngineModule.hpp"
#include "engine/Filesystem/NativeFileSystem.hpp"
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

	if (!m_MismatchedResourcesPool.empty())
	{
		auto it = m_MismatchedResourcesPool.front();
		m_MismatchedResourcesPool.pop();

		auto& main_instance = ApplicationSingleton::Instance();
		fs::IFileSystem* resource_fs = main_instance.GetFilesystemManager()->Get("resources");

			std::shared_ptr<fs::IFile> file = resource_fs->OpenFile(it, fs::io::OpenMode::In);

			if (file)
			{
			switch (file->GetType())
			{
				case fs::IFile::EType::JSON:
				{
					nlohmann::json json;

					if (file->Size() > 0)
					{
						file->Read(json, file->Size());
					}

					unsigned int fs_mark_hash = json["fs_mark"];
					fs::EFilesystemMark fs_mark = fs::EFilesystemMarkFromHash(fs_mark_hash);

					switch (fs_mark)
					{
					case fs::EFilesystemMark::Level:
					{
						main_instance.GetWorld()->EmplaceInitialLevelData(json);
						break;
					}
					default:
					{
						break;
					}
					}
				}
				}
			}
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

void EngineModule::InitializeAssets()
{
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
					default:
					{
						std::cout << "WARNING: File with alias: " << alias << " has an invalid type and couldn't be assigned to any pool.\nData from this file will be processed later.";
						m_MismatchedResourcesPool.push(alias);
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

