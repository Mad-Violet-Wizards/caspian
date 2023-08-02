#include "engine/pch.hpp"

#include <iostream>

#include "ToolsImpl.hpp"
#include "AssetWindows.hpp"

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
			constexpr auto assets_popup_name = "AssetsPopup";

			if (ImGui::Button("Assets", styles.toolbar_button_size))
				ImGui::OpenPopup(assets_popup_name);

			if (ImGui::BeginPopup(assets_popup_name))
			{
				if (ImGui::Selectable("Import Asset"))
				{
					m_Manager->m_ImportAssetWindow.m_Active = true;
				}

				if (ImGui::Selectable("View Assets List"))
				{
					m_Manager->m_AssetListWindow.m_Active = true;
				}

				ImGui::EndPopup();
			}
		}
		
		{
			constexpr auto cheats_popup_name = "CheatsPopup";

			ImGui::SameLine();
			if (ImGui::Button("Cheats", styles.toolbar_button_size))
				ImGui::OpenPopup(cheats_popup_name);

			if (ImGui::BeginPopup(cheats_popup_name))
				ImGui::EndPopup();
		}

		{
			constexpr auto npc_popup_name = "NPCPopup";

			ImGui::SameLine();
			if (ImGui::Button("NPC", styles.toolbar_button_size))
				ImGui::OpenPopup(npc_popup_name);

			if (ImGui::BeginPopup(npc_popup_name))
				ImGui::EndPopup();
		}
		
		{
			constexpr auto level_popup_name = "LevelPopup";

			ImGui::SameLine();
			if (ImGui::Button("Level", styles.toolbar_button_size))
				ImGui::OpenPopup(level_popup_name);

			if (ImGui::BeginPopup(level_popup_name))
				ImGui::EndPopup();
		}

		{
			constexpr auto project_popup_name = "ProjectPopup";

			ImGui::SameLine();
			if (ImGui::Button("Project", styles.toolbar_button_size))
				ImGui::OpenPopup(project_popup_name);

			if (ImGui::BeginPopup(project_popup_name))
			{
				if (ImGui::Selectable("New..."))
				{
					m_Manager->m_NewProjectWindow.m_Active = true;
				}

				if (ImGui::Selectable("Load..."))
				{
					// Action.
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
	, m_NotificationManager(this)
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
}

void Manager::ShowNotification(ENotificationType _type, std::string_view _msg)
{
	m_NotificationManager.ShowNotification(_type, _msg);
}

void Manager::OnCreateNewProject(std::string_view _project_name, std::string_view _project_path)
{
	std::cout << "Creating new project: " << _project_name << " at " << _project_path << "\n";

	// TODO: Call low API to create new project.
	// We need some kind of callback to show the user that the project was created successfully.

	const bool project_created_successfully = true;

	if (project_created_successfully)
	{
		ShowNotification(ENotificationType::Success, "Project created succesfully! :)");
		// Show info notification.
	}
}