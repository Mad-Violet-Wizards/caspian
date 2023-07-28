#include "engine/pch.hpp"

#include "ImGuiProjectTools.hpp"

#include <iostream>
#include <imgui-file-dialog/ImGuiFileDialog.h>
#include "engine/Tools/ImGuiUtils.hpp"
#include "engine/Tools/ImGuiTools.hpp"

using namespace Tools;

/////////////////////////////////////////////////////////
/* ImGuiNewProjectWindow*/

ImGuiNewProjectWindow::ImGuiNewProjectWindow(ImGuiManager* _mgr)
	: m_Manager(_mgr) {}

void ImGuiNewProjectWindow::Render()
{
	if (!m_Active)
		return;

	if (ImGui::Begin("New Project ", &m_Active, ImGuiWindowFlags_AlwaysAutoResize))
	{
		constexpr auto max_input_sizes = 128;
		m_projectName.resize(max_input_sizes);
		m_projectPath.resize(max_input_sizes);

		ImGui::InputText("Name", &m_projectName[0], max_input_sizes);
		ImGui::InputText("Path", &m_projectPath[0], max_input_sizes);
		
		ImGui::Text("Select the directory where project files are going to be initialized.");
		ImGui::Text("TIP: Create a new project folder in ROOT directory - where the Caspian.sln lays.\nEnter the folder and create new directory here with the same name as the project name.");

		constexpr auto dialog_name = "ProjectPathDialog";
		if (ImGui::Button("Browse"))
		{
			ImGuiFileDialog::Instance()->OpenDialog(dialog_name, "Choose new project directory", nullptr, ".");
		}

		if (ImGuiFileDialog::Instance()->Display(dialog_name))
		{
			if (ImGuiFileDialog::Instance()->IsOk())
			{
				static auto paths_helpers = utils::paths();

				m_projectPath = paths_helpers.TrimToRootPath(ImGuiFileDialog::Instance()->GetCurrentPath());
			}

			ImGuiFileDialog::Instance()->Close();
		}

		{
			static auto styles = utils::styles();
			styles.push_accept_button_style();

			ImGui::Dummy(ImVec2(0.f, 10.f));
			if (ImGui::Button("Create"))
			{
				if (m_Manager)
					m_Manager->OnCreateNewProject(m_projectName, m_projectPath);

				m_Active = false;
			}

			styles.pop_accept_button_style();

			styles.push_cancel_button_style();
			ImGui::SameLine();
			if (ImGui::Button("Cancel"))
				m_Active = false;
			styles.pop_cancel_button_style();

		}
	}
	ImGui::End();
}

/////////////////////////////////////////////////////////
/* ImGuiOpenProjectWindow*/
void ImGuiLoadProjectAction::operator()()
{
}