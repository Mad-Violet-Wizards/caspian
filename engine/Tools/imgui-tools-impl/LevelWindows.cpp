#include "engine/pch.hpp"

#include "LevelWindows.hpp"
#include "ToolsImpl.hpp"

#include <imgui-file-dialog/ImGuiFileDialog.h>
#include <imgui/misc/cpp/imgui_stdlib.h>

using namespace Tools_Impl;

/////////////////////////////////////////////////////////
/* NewLevelWindow */
void NewLevelWindow::Render()
{
	if (!m_Active)
		return;

	if (ImGui::Begin("New Level", &m_Active))
	{
		ImGui::InputText("Name", &m_LevelName);
		ImGui::InputText("Path", &m_LevelPath); 
		ImGui::SameLine();
	
		constexpr auto dlg_name = "NewLevelWindowPathDialog";
		if (ImGui::Button("Select path"))
		{
			auto& engine_module = ApplicationSingleton::Instance().GetEngineModule();
			auto project_path = engine_module.GetCurrentProject().m_ProjectPath;
			project_path += "\\" + engine_module.GetCurrentProject().m_ProjectName + "\\";

			ImGuiFileDialog::Instance()->OpenDialog(dlg_name, "Choose directory", nullptr, project_path);
		}

		if (ImGuiFileDialog::Instance()->Display(dlg_name))
		{
			if (ImGuiFileDialog::Instance()->IsOk())
			{
				m_LevelPath = ImGuiFileDialog::Instance()->GetFilePathName();
			}

			ImGuiFileDialog::Instance()->Close();
		}

		ImGui::InputText("Tile width", &m_nTileWidth);
		ImGui::InputText("Tile height", &m_nTileHeight);

		auto fn_filter_digit_only_input = [](std::string_view _in) -> bool
		{
				return std::all_of(_in.begin(), _in.end(), [](char c) { return std::isdigit(c); });
		};

		const bool bTileWidthOk = fn_filter_digit_only_input(m_nTileWidth);
		const bool bTileHeightOk = fn_filter_digit_only_input(m_nTileHeight);

		if (!bTileWidthOk) ImGui::TextColored(ImVec4(1.f, 0.f, 0.f, 1.f), "Tile width must be a number!");
		if (!bTileHeightOk) ImGui::TextColored(ImVec4(1.f, 0.f, 0.f, 1.f), "Tile height must be a number!");

		if (ImGui::Button("Create"))
		{

		}

		ImGui::End();
	}
}