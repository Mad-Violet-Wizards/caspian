#include "engine/pch.hpp"

#include <iostream>

#include "engine/Tools/ImGuiTools.hpp"
#include "engine/Tools/ImGuiAssetsTools.hpp"

using namespace Tools;

////////////////////////////////////////////////////////
/* ImGuiToolbar */
ImGuiToolbar::ImGuiToolbar(ImGuiManager* _mgr)
	: m_Manager(_mgr)
{
}

void ImGuiToolbar::Render()
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
/* ImGuiManager */
ImGuiManager::ImGuiManager()
	: m_Toolbar(this)
	, m_NewProjectWindow(this)
{

}

void ImGuiManager::ToggleActiveState()
{
	m_Active					 = !m_Active;
	m_Toolbar.m_Active = !m_Toolbar.m_Active;
}

void ImGuiManager::Update(float _dt)
{
	if (!m_Active)
		return;

	//m_Toolbar.Update(_dt);
	//m_ImportAssetWindow.Update(_dt);
	//m_AssetListWindow.Update(_dt);
}

void ImGuiManager::Render()
{
	if (!m_Active)
		return;
	
	m_Toolbar.Render();
	m_ImportAssetWindow.Render();
	m_AssetListWindow.Render();
	m_NewProjectWindow.Render();
}

void ImGuiManager::OnCreateNewProject(const std::string_view _project_name, const std::string_view _project_path)
{
	std::cout << "Creating new project: " << _project_name << " at " << _project_path << "\n";
}
