#include "engine/pch.hpp"

#include <iostream>

#include "ToolsImpl.hpp"
#include "AssetWindows.hpp"
#include "engine/Filesystem/NativeFileSystem.hpp"
#include "engine/Filesystem/NativeFile.hpp"
#include <vendor/include/nlohmann/json.hpp>
#include "engine/Core/Level.hpp"
#include "engine/Filesystem/FilesystemMark.hpp"
#include "engine/Core/Serializable/ProjectSerializable.hpp"
#include "engine/Core/Serializable/LevelSerializable.hpp"
#include "engine/Core/Serializable/AssetsSerializable.hpp"
#include <cereal/archives/portable_binary.hpp>

using namespace Tools_Impl;

////////////////////////////////////////////////////////
/* Toolbar */
Toolbar::Toolbar(Manager* _mgr)
	: IWindow(_mgr) { }

void Toolbar::Render()
{
	if (!m_Active)
		return;

	const bool bAssets_storage_initialized = ApplicationSingleton::Instance().GetEngineModule().GetAssetsStorage()->IsInitialized();

	static auto flags = utils::flags();

	ImGui::Begin("Toolbar", &m_Active, flags.toolbar_window_flags);
	{
		ImGui::SetWindowPos(ImVec2(0, 0));
		ImGui::SetWindowSize(ImVec2(ImGui::GetIO().DisplaySize.x, 32));

		static auto styles = utils::styles();

		styles.toolbar_push_combobox_style();
		styles.toolbar_push_button_style();

		{
			constexpr auto files_popup_name = "FilePopup";

			ImGui::SameLine();
			if (ImGui::Button("File", styles.toolbar_button_size))
				ImGui::OpenPopup(files_popup_name);

			if (ImGui::BeginPopup(files_popup_name))
			{
				if (ImGui::Selectable("New project..."))
				{
					m_Manager->m_NewProjectWindow.m_Active = true;
				}

				if (ImGui::Selectable("Load project..."))
				{
					m_Manager->m_LoadProjectWindow.m_Active = true;
				}

				if (ImGui::Selectable("Import asset..."))
				{
					if (!ApplicationSingleton::Instance().GetProjectsManager()->IsAnyProjectLoaded())
					{
						m_Manager->m_NotificationManager.ShowNotification(ENotificationType::Error, "Cannot import asset\nwithout loading project first!");
					}
					else
					{
						m_Manager->m_ImportAssetWindow.m_Active = true;
					}
				}

				if (ImGui::Selectable("Assets list"))
				{
					if (bAssets_storage_initialized)
					{
						m_Manager->m_AssetListWindow.m_Active = true;
					}
					else
					{
						m_Manager->m_NotificationManager.ShowNotification(ENotificationType::Error, "Cannot open assets list\nwithout loading project first!");
					}
				}

				ImGui::EndPopup();
			}
		}

		{
			constexpr auto levels_popup_name = "LevelsPopup";

		ImGui::SameLine();
			if (ImGui::Button("Tools", styles.toolbar_button_size))
				ImGui::OpenPopup(levels_popup_name);

			if (ImGui::BeginPopup(levels_popup_name))
			{
				if (ImGui::Selectable("Level editor..."))
				{
					if (bAssets_storage_initialized)
					{
						m_Manager->m_LevelEditorWindow.m_Active = true;
					}
					else
					{
						m_Manager->m_NotificationManager.ShowNotification(ENotificationType::Error, "Cannot create new level\nwithout loading project first!");
					}
				}

				ImGui::EndPopup();
			}
			
		}
		
		styles.toolbar_pop_combobox_style();
		styles.toolbar_pop_button_style();

	}
	ImGui::End();
}

////////////////////////////////////////////////////////
/* ImGuiNotificationManager */
NotificationsManager::NotificationsManager(Manager* _mgr)
	: m_Manager(_mgr)
	, m_ErrorWindow(std::make_unique<notifications::ErrorNotificationWindow>(_mgr))
	, m_WarningWindow(std::make_unique<notifications::WarningNotificationWindow>(_mgr))
	, m_SuccessWindow(std::make_unique<notifications::SuccessNotificationWindow>(_mgr))
{
}

void NotificationsManager::Update(float _dt)
{
	m_ErrorWindow->Update(_dt);
	m_WarningWindow->Update(_dt);
	m_SuccessWindow->Update(_dt);
}

void NotificationsManager::Render()
{
	m_ErrorWindow->Render();
	m_WarningWindow->Render();
	m_SuccessWindow->Render();
}

void NotificationsManager::ShowNotification(ENotificationType _type, std::string_view _msg)
{
	int active_notifications = 0;

	active_notifications += m_ErrorWindow->m_Active ? 1 : 0;
	active_notifications += m_WarningWindow->m_Active ? 1 : 0;
	active_notifications += m_SuccessWindow->m_Active ? 1 : 0;

	switch (_type)
	{
		case ENotificationType::None:
		{
			// TODO: Call low API error log/msg.
			break;
		}
		case ENotificationType::Warning:
		{
			m_WarningWindow->SetMessage(_msg);
			m_WarningWindow->Show(active_notifications);
			break;
		}
		case ENotificationType::Error:
		{
			m_ErrorWindow->SetMessage(_msg);
			m_ErrorWindow->Show(active_notifications);
			break;
		}
		case ENotificationType::Success:
		{
			m_SuccessWindow->SetMessage(_msg);
			m_SuccessWindow->Show(active_notifications);
			break;
		}
	}
}

void NotificationsManager::ClearNotifications()
{
	m_ErrorWindow->Hide();
	m_WarningWindow->Hide();
	m_SuccessWindow->Hide();
}

void NotificationsManager::OnNotificationHidden()
{
	std::vector<INotificationWindow*> active_notification_vec;

	if (m_ErrorWindow->m_Active)
		active_notification_vec.push_back(m_ErrorWindow.get());

	if (m_WarningWindow->m_Active)
		active_notification_vec.push_back(m_WarningWindow.get());

	if (m_SuccessWindow->m_Active)
		active_notification_vec.push_back(m_SuccessWindow.get());

	// Sort vector by descending m_DisplayTime value.
	std::sort(active_notification_vec.begin(), active_notification_vec.end(), [](const auto& _lhs, const auto& _rhs)
	{
		return _lhs->m_DisplayTime > _rhs->m_DisplayTime;
	});

	// Recalculate positions.
	for (int i = 0; i < active_notification_vec.size(); ++i)
	{
		auto* notification = active_notification_vec[i];
		notification->m_Pos = notification->CalculatePosition(i);
	}

}

////////////////////////////////////////////////////////
/* Manager */
Manager::Manager()
	: m_Toolbar(this)
	, m_ImportAssetWindow(this)
	, m_AssetListWindow(this)
	, m_NewProjectWindow(this)
	, m_LoadProjectWindow(this)
	, m_NotificationManager(this)
	, m_LevelEditorWindow(this)
{

}

void Manager::ToggleActiveState()
{
	m_Active					 = !m_Active;
	m_Toolbar.m_Active = !m_Toolbar.m_Active;
}

void Manager::Update(float _dt)
{
	if (!m_Active)
		return;

	m_NotificationManager.Update(_dt);
	m_AssetListWindow.Update(_dt);
	m_LevelEditorWindow.Update(_dt);

	//m_Toolbar.Update(_dt);
	//m_ImportAssetWindow.Update(_dt);
	//m_AssetListWindow.Update(_dt);
}

void Manager::Render()
{
	if (!m_Active)
		return;
	
	m_Toolbar.Render();

	m_NotificationManager.Render();

	m_ImportAssetWindow.Render();
	m_AssetListWindow.Render();
	m_NewProjectWindow.Render();
	m_LoadProjectWindow.Render();

	m_LevelEditorWindow.Render();
}

void Manager::ShowNotification(ENotificationType _type, std::string_view _msg)
{
	m_NotificationManager.ShowNotification(_type, _msg);
}

void Manager::CreateNewProjectRequest(const std::string& _project_name, const std::string& _project_path)
{	
	bool file_closed_successfully = false;

	auto& main_instance = ApplicationSingleton::Instance();

	// TODO: Refactor to use cereal.
	fs::IFileSystem* appdata_fs = main_instance.GetEngineModule().GetFilesystemManager()->Get(Windows::S_ENGINE_APPDATA_ALIAS);

	if (!appdata_fs->FileExists("projects.json"))
	{
		bool projects_json_created = appdata_fs->CreateFile("projects.json", fs::IFile::EType::JSON);

		if (!projects_json_created)
		{
			std::cout << "DEBUG: [ToolsImpl] Failed to create projects.json file.\n";
			return;
		}
	}

	Serializable::JSON::ProjectInfo project_info(_project_name, _project_path);

	if (std::shared_ptr<fs::IFile> projects_json_file = appdata_fs->OpenFile("projects.json", fs::io::OpenMode::ReadWrite))
	{
		Projects::Manager* projects_manager = main_instance.GetProjectsManager();
		projects_manager->PushProject(project_info);

		std::vector<Serializable::JSON::ProjectInfo> projects_info = projects_manager->GetProjectsInfo();

		std::shared_ptr<ISerializable::JSON> projects_json = std::make_shared<Serializable::JSON::ProjectsInfo>(projects_info);

		projects_json_file->Seek(0, fs::io::Origin::Begin);
		projects_json_file->SerializeJson(projects_json);

		file_closed_successfully = appdata_fs->CloseFile(projects_json_file);
	}

	bool project_folder_created = false;
	bool resource_folder_created = false;
	bool data_folder_created = false;
	bool magic_number_file_created = false;

	if (std::filesystem::exists(_project_path))
	{
		std::filesystem::path project_path{ _project_path };
		project_path /= _project_name;

		project_folder_created = std::filesystem::create_directories(project_path);

		if (project_folder_created)
		{
			std::filesystem::path resource_path{ project_path };
			resource_path /= "resources";
			resource_folder_created = std::filesystem::create_directories(resource_path);

			std::filesystem::path data_path{ project_path };
			data_path /= "data";
			data_folder_created = std::filesystem::create_directories(data_path);

			Serializable::Binary::MagicFileInfo magic_file_info(ApplicationSingleton::Instance().GetEngineModule().GetEngineVersion());

			std::filesystem::path magic_number_path{ project_path };
			magic_number_path /= _project_name + ".casp";

			std::fstream magic_number_file;
			magic_number_file.open(magic_number_path, std::fstream::out);

			cereal::PortableBinaryOutputArchive archive(magic_number_file);
			archive(magic_file_info);

			if (magic_number_file.good())
				magic_number_file_created = true;

			magic_number_file.close();
		}
	}

	const bool success = file_closed_successfully && project_folder_created && resource_folder_created && data_folder_created && magic_number_file_created;

	if (success)
	{
		ShowNotification(ENotificationType::Success, "Project created succesfully! :)");
	}
	else
	{
		ShowNotification(ENotificationType::Error, "Failed to create project! :(");
		main_instance.GetProjectsManager()->RemoveProject(_project_name);
	}
}

void Manager::LoadProjectRequest(const std::string& _project_name, const std::string& _project_path)
{
	std::filesystem::path project_path{ _project_path };
	project_path.append(_project_name);

	if (std::filesystem::exists(project_path))
	{
		const std::filesystem::path resources_path{ project_path / "resources" };
		const std::filesystem::path data_path{ project_path / "data" };
		const std::array<unsigned char, 4> magic_numbers = { 'C', 'A', 'S', 'P' };

		// Read magic file.
		std::fstream magic_number_file;
		magic_number_file.open(project_path / (_project_name + ".casp"), std::fstream::in);

		Serializable::Binary::MagicFileInfo magic_file_info;
		cereal::PortableBinaryInputArchive archive(magic_number_file);
		archive(magic_file_info);

		if (magic_file_info.m_MagicNumber != magic_numbers)
		{
			ShowNotification(ENotificationType::Error, "Invalid project file!");
			return;
		}
		auto& engine_module = ApplicationSingleton::Instance().GetEngineModule();

		if (magic_file_info.m_EngineVersion != engine_module.GetEngineVersion())
		{
			ShowNotification(ENotificationType::Error, "Project was created with different engine version!");
			return;
		}

		Projects::Manager* project_manager = ApplicationSingleton::Instance().GetProjectsManager();

		bool project_set = project_manager->SetCurrentProject(_project_name);

		if (project_set)
		{
			auto msg = std::format("Project {} parsed successfully. Loading systems.", _project_name);
			ShowNotification(ENotificationType::Success, msg);
			ApplicationSingleton::Instance().UpdateWindowTitle(std::format("CASPIAN ENGINE | {}", _project_name));
		}
	}
}

void Manager::CreateNewLevelRequest(const std::string& _lvl_path, const std::string& _lvl_name, unsigned int _tile_width, unsigned int _tile_height)
{
	auto& main_instance = ApplicationSingleton::Instance();

	fs::IFileSystem* resource_fs = main_instance.GetEngineModule().GetFilesystemManager()->Get("resources");
	
	bool json_created_succesfully = false;

	std::string relative_lvl_path{ _lvl_path };
	relative_lvl_path = relative_lvl_path.substr(resource_fs->GetPath().size() + 1);

	if (!resource_fs->FileExists(relative_lvl_path))
	{
		json_created_succesfully = resource_fs->CreateFile(relative_lvl_path, fs::IFile::EType::JSON);

		if (!json_created_succesfully)
		{
			std::cout << "DEBUG: [ToolsImpl] Failed to create level file: " << relative_lvl_path << std::endl;
		}
	}

	bool file_closed_successfully = false;

	if (std::shared_ptr<fs::IFile> level_json_file = resource_fs->OpenFile(relative_lvl_path, fs::io::OpenMode::ReadWrite))
	{
		const std::string& root_chunk_file_name = _lvl_name + ".rootchunk";
		std::shared_ptr<ISerializable::JSON> level_data = std::make_shared<Serializable::JSON::LevelInfo>(_lvl_name, root_chunk_file_name, _tile_width, _tile_height);
		level_json_file->SerializeJson(level_data);

		std::shared_ptr<Serializable::JSON::LevelInfo> level_info = std::dynamic_pointer_cast<Serializable::JSON::LevelInfo>(level_data);
		main_instance.GetWorld()->PushInitialLevelData(level_info);

		file_closed_successfully = resource_fs->CloseFile(level_json_file);
	}

	std::fstream chunk_file;

	fs::IFileSystem* data_fs = main_instance.GetEngineModule().GetFilesystemManager()->Get("data");

	if (!data_fs->FileExists("levels"))
	{
				data_fs->CreateFile("levels", fs::IFile::EType::Directory);
	}

	std::filesystem::path chunk_file_path{ data_fs->GetPath() };
	chunk_file_path /= "levels";
	chunk_file_path /= _lvl_name + ".rootchunk";

	chunk_file.open(chunk_file_path, std::fstream::out | std::fstream::binary);

	const bool chunk_file_created = chunk_file.is_open();

	chunk_file.close();

	const bool success = json_created_succesfully && file_closed_successfully && chunk_file_created;

	if (success)
	{
		ShowNotification(ENotificationType::Success, "Level created succesfully! :)");
	}
	else
	{
		ShowNotification(ENotificationType::Error, "Failed to create level! :(");
	}
}

void Manager::OpenAssetTableForAction(IAssetsTableActionsListener* _listener)
{
	if (!_listener)
		return;

	if (!m_AssetListWindow.m_Active)
		m_AssetListWindow.m_Active = true;

	m_AssetListWindow.OnOpenForAction(_listener);
}

void Manager::AddTilesetRequest(const std::string& _tileset_key, const std::string& _tileset_name, unsigned int _tile_width, unsigned int _tile_height)
{
	std::cout << "=======================\n";
	std::cout << "Adding new tileset\n";
	std::cout << "Key (assets storage): " << _tileset_key << "\n";
	std::cout << "Name: " << _tileset_name << "\n";
	std::cout << "Width: " << _tile_width << "\n";
	std::cout << "Height: " << _tile_height << "\n";


	auto& main_instance = ApplicationSingleton::Instance();

	fs::IFileSystem* data_fs = main_instance.GetEngineModule().GetFilesystemManager()->Get("data");
	const std::string tilemaps_storage_path = "tilemaps\\data.tilemaps";

	bool tilemap_file_created = false;
	bool tilemap_file_closed = false;

	if (!data_fs->FileExists(tilemaps_storage_path))
	{
		tilemap_file_created = data_fs->CreateFile(tilemaps_storage_path, fs::IFile::EType::Data_Tilemaps);
	}

	if (std::shared_ptr<fs::IFile> tilemap_file = data_fs->OpenFile(tilemaps_storage_path, fs::io::OpenMode::ReadWrite))
	{
		Assets::TilemapStorage* tilemap_storage = main_instance.GetEngineModule().GetAssetsStorage()->GetTilemapStorage();
		
		const Serializable::Binary::TilesetInfo tileset_info(_tileset_key, _tileset_name, _tile_width, _tile_height);
		tilemap_storage->PushTilesetInfo(tileset_info);
		tilemap_file->Seek(0, fs::io::Origin::Begin);
		tilemap_file->SerializeBinary(tilemap_storage->GetTilesetsInfo());

		tilemap_file_closed = data_fs->CloseFile(tilemap_file);
	}

	if (tilemap_file_closed)
	{
		ShowNotification(ENotificationType::Success, "Tileset added succesfully! :)");
	}
	else
	{
		ShowNotification(ENotificationType::Error, "Failed to add tileset! :(");
	}
}
