#include "engine/pch.hpp"

#include <imgui-file-dialog/ImGuiFileDialog.h>
#include <iostream>

#include "engine/Tools/ImGuiTools.hpp"

using namespace Tools;

////////////////////////////////////////////////////////
/* ImGuiToolbar */
ImGuiToolbar::ImGuiToolbar(ImGuiManager* _mgr)
	: m_Manager(_mgr)
{
}

void ImGuiToolbar::Update(float _dt)
{
	if (!m_Active)
		return;
}

void ImGuiToolbar::Render()
{
	if (!m_Active)
		return;

	static auto flags = utils::flags();

	ImGui::Begin("Toolbar", &m_Active, flags.toolbar_window_flags);
	{
		ImGui::SetWindowPos(ImVec2(0, 0));
		ImGui::SetWindowSize(ImVec2(ImGui::GetIO().DisplaySize.x, 60));

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
					// Action.
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
			const char* npc_popup_name = "NPCPopup";

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

		styles.toolbar_pop_combobox_style();
		styles.toolbar_pop_button_style();

	}
	ImGui::End();
}

/////////////////////////////////////////////////////////
/* ImGuiImportAssetWindow */
void ImGuiImportAssetWindow::Update(float _dt)
{

}

void ImGuiImportAssetWindow::Render()
{
	if (!m_Active)
		return;

	if (ImGui::Begin("Import Asset Window", &m_Active))
	{
		constexpr auto dialog_name = "ImportAssetFileDialog";
		if (ImGui::Button("Select file"))
		{
			ImGuiFileDialog::Instance()->OpenDialog(dialog_name, "Choose File", ".cpp,.h,.hpp", ".");
		}

		if (ImGuiFileDialog::Instance()->Display(dialog_name))
		{
			if (ImGuiFileDialog::Instance()->IsOk())
			{
				const std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
				const std::string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();

				std::cout << "[ImGuiImportAssetWindow] File path name: " << filePathName << "\n";
				std::cout << "[ImGuiImportAssetWindow] File path: " << filePath << "\n";
			}

			ImGuiFileDialog::Instance()->Close();
		}
	}

	ImGui::End();
}

////////////////////////////////////////////////////////
/* ImGuiManager */
ImGuiManager::ImGuiManager()
	: m_Toolbar(this)
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

	m_Toolbar.Update(_dt);
	m_ImportAssetWindow.Update(_dt);
}

void ImGuiManager::Render()
{
	if (!m_Active)
		return;
	
	m_Toolbar.Render();
	m_ImportAssetWindow.Render();
}
