#include "engine/pch.hpp"

#include "ImGuiProjectTools.hpp"
#include "ImGuiUtils.hpp"
#include "ImGuiTools.hpp"
#include "ImGuiValidators.hpp"

#include <iostream>
#include <imgui-file-dialog/ImGuiFileDialog.h>

using namespace Tools;

/////////////////////////////////////////////////////////
/* ImGuiNewProjectWindow*/
ImGuiNewProjectWindow::ImGuiNewProjectWindow(ImGuiManager* _mgr)
	: ImGuiIWindow(_mgr) { }

void ImGuiNewProjectWindow::Render()
{
	if (!m_Active)
		return;

	if (ImGui::Begin("New Project ", &m_Active, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoBringToFrontOnFocus))
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
				const bool project_name_valid = validate_input::is_valid_project_name(m_projectName);
				const bool project_path_valid = validate_input::is_valid_project_path(m_projectPath);

				std::string error_msg;

				if (!project_name_valid)
					error_msg += "Invalid project name.\n";

				if (!project_path_valid)
					error_msg += "Invalid project path.\n";

				if (!project_name_valid || !project_path_valid)
					m_Manager->ShowNotification(ENotificationType::Error, error_msg);

				if (project_name_valid && project_path_valid)
				{
					m_Manager->OnCreateNewProject(m_projectName, m_projectPath);
				}
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