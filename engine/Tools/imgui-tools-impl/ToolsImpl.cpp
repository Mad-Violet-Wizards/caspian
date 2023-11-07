#include "engine/pch.hpp"

#include <iostream>

#include "ToolsImpl.hpp"
#include "AssetWindows.hpp"
#include "game/Application.hpp"
#include "engine/Filesystem/NativeFileSystem.hpp"
#include "engine/Filesystem/NativeFile.hpp"
#include "vendor/include/nlohmann/json.hpp"

using namespace Tools_Impl;

////////////////////////////////////////////////////////
/* Toolbar */
Toolbar::Toolbar(Manager* _mgr)
	: IWindow(_mgr) { }

void Toolbar::Render()
{
	if (!m_Active)
		return;

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
					if (!ApplicationSingleton::Instance().GetEngineModule().IsAnyProjectLoaded())
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
					if (m_Manager->m_AssetListWindow.GetAvailability())
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
	, m_MagicNumberBytes{ 0x43, 0x41, 0x53, 0x50 }
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
}

void Manager::ShowNotification(ENotificationType _type, std::string_view _msg)
{
	m_NotificationManager.ShowNotification(_type, _msg);
}

void Manager::CreateNewProjectRequest(const std::string& _project_name, const std::string& _project_path)
{	
	// JSON PART:
	bool data_packed_in_json = false;
	bool file_closed_successfully = false;

	auto& main_instance = ApplicationSingleton::Instance();

	fs::IFileSystem* appdata_fs = main_instance.GetFilesystemManager()->Get(Windows::S_ENGINE_APPDATA_ALIAS);

	if (!appdata_fs->FileExists("projects.json"))
	{
		bool projects_json_created = appdata_fs->CreateFile("projects.json", fs::IFile::EType::JSON);

		if (!projects_json_created)
		{
			std::cout << "DEBUG: [ToolsImpl] Failed to create projects.json file.\n";
			return;
		}
	}

	if (std::shared_ptr<fs::IFile> projects_json_file = appdata_fs->OpenFile("projects.json", fs::io::OpenMode::ReadWrite))
	{
		nlohmann::json projects_json;

		if (projects_json_file->Size() > 0)
			projects_json_file->Read(projects_json, projects_json_file->Size());

		nlohmann::json project_obj = {
			{ "name", _project_name },
			{ "path", _project_path }
		};

		projects_json["projects"].push_back(project_obj);

		projects_json_file->Seek(0, fs::io::Origin::Begin);
		data_packed_in_json = projects_json_file->Write(projects_json, 0);

		file_closed_successfully = appdata_fs->CloseFile(projects_json_file);
	}

	// TODO: FILESYSTEM PART:
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

			const std::array<unsigned char, 4> magic_number_bytes = m_MagicNumberBytes;
			const std::array<unsigned char, 4> engine_version = ApplicationSingleton::Instance().GetEngineModule().GetEngineVersion();

			// We do not have the project loaded yet, so no access to resource & data filesystems.
			std::filesystem::path magic_number_path{ project_path };
			magic_number_path /= _project_name + ".casp";

			std::fstream magic_number_file;
			magic_number_file.open(magic_number_path, std::fstream::out);

			magic_number_file_created = magic_number_file.is_open();

			if (magic_number_file_created)
			{
				magic_number_file.write(reinterpret_cast<const char*>(magic_number_bytes.data()), magic_number_bytes.size());
				magic_number_file.write(reinterpret_cast<const char*>(engine_version.data()), engine_version.size());
				magic_number_file.close();
			}
		}
	}

	const bool success = data_packed_in_json && file_closed_successfully && project_folder_created && resource_folder_created && data_folder_created && magic_number_file_created;

	if (success)
	{
		ShowNotification(ENotificationType::Success, "Project created succesfully! :)");
	}
	else
	{
		ShowNotification(ENotificationType::Error, "Failed to create project! :(");
	}
}

void Manager::LoadProjectRequest(const std::string& _project_name, const std::string& _project_path)
{
	std::filesystem::path project_path{ _project_path };
	project_path.append(_project_name);

	if (std::filesystem::exists(project_path))
	{
		const std::filesystem::path resources_path{ project_path / "resources" };
		const std::filesystem::path data_path { project_path / "data" };

		// Read magic file.
		std::fstream magic_number_file;
		magic_number_file.open(project_path / (_project_name + ".casp"), std::fstream::in);

		std::array<unsigned char, 4> magic_number_bytes;
		std::array<unsigned char, 4> engine_version;

		magic_number_file.read(reinterpret_cast<char*>(magic_number_bytes.data()), magic_number_bytes.size());
		magic_number_file.read(reinterpret_cast<char*>(engine_version.data()), engine_version.size());

		// Validate magic number.
		if (magic_number_bytes != m_MagicNumberBytes)
		{
			ShowNotification(ENotificationType::Error, "Invalid project file!");
			return;
		}

		auto& engine_module = ApplicationSingleton::Instance().GetEngineModule();

		if (engine_version != engine_module.GetEngineVersion())
		{
			ShowNotification(ENotificationType::Error, "Project was created with different engine version!");
			// TODO: Depending on what will change in the future handle exceptions.
		}

		if (engine_module.IsAnyProjectLoaded())
		{
			// TOOD: Ask user if he wants to switch.
			engine_module.UnloadCurrentProject();
		}

		Project p;
		p.m_ProjectName = _project_name;
		p.m_ProjectPath = _project_path;

		engine_module.SetCurrentProject(p);

		if (p == engine_module.GetCurrentProject())
		{
			auto msg = std::format("Project {} parsed successfully. Loading systems.", _project_name);
			ShowNotification(ENotificationType::Success, msg);
			ApplicationSingleton::Instance().UpdateWindowTitle(std::format("CASPIAN ENGINE | {}", _project_name));
		}
	}
}

void Manager::OnAssetsStorageInitialized()
{
	m_AssetListWindow.SetAvailability(true);
}
