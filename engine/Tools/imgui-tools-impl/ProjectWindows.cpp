#include "engine/pch.hpp"

#include "ProjectWindows.hpp"
#include "Utils.hpp"
#include "ToolsImpl.hpp"
#include "Validators.hpp"

#include <iostream>
#include <imgui-file-dialog/ImGuiFileDialog.h>

using namespace Tools_Impl;

/////////////////////////////////////////////////////////
/* NewProjectWindow*/
NewProjectWindow::NewProjectWindow(Manager* _mgr)
	: IWindow(_mgr) { }

void NewProjectWindow::Render()
{
	if (!m_Active)
		return;

	if (ImGui::Begin("New Project ", &m_Active, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoBringToFrontOnFocus))
	{
		constexpr auto max_input_sizes = 128;
		m_projectName.resize(max_input_sizes);
		m_projectPath.resize(max_input_sizes);

		if (ImGui::InputText("Name", &m_projectName[0], max_input_sizes))
		{
			m_projectName.resize(strlen(&m_projectName[0]));
		}

		if (ImGui::InputText("Path", &m_projectPath[0], max_input_sizes))
		{
			m_projectPath.resize(strlen(&m_projectPath[0]));
		}
		
		ImGui::Text("Engine will create new directory under path you provided.\nName of directory is equals to name of project.");

		constexpr auto dialog_name = "ProjectPathDialog";
		if (ImGui::Button("Browse"))
		{
			ImGuiFileDialog::Instance()->OpenDialog(dialog_name, "Choose new project directory", nullptr, ".");
		}

		if (ImGuiFileDialog::Instance()->Display(dialog_name))
		{
			if (ImGuiFileDialog::Instance()->IsOk())
			{
				m_projectPath = ImGuiFileDialog::Instance()->GetCurrentPath();
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
					error_msg += "Invalid project name. \nEmpty, or invalid character used.\n";

				if (!project_path_valid)
					error_msg += "Invalid project path. \nPlease use caspian/projects/ dir.\n";

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
//void ImGuiLoadProjectAction::operator()()
//{
//}