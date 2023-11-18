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
				return std::all_of(_in.begin(), _in.end(), [](char c) { return std::isdigit(c); }) && !_in.empty();
		};

		const bool bTileWidthOk = fn_filter_digit_only_input(m_nTileWidth);
		const bool bTileHeightOk = fn_filter_digit_only_input(m_nTileHeight);
		const bool bPatchValid = std::filesystem::exists(m_LevelPath);

		const ImVec4 red_color { 1.f, 0.f, 0.f, 1.f };

		if (!bTileWidthOk && !m_nTileHeight.empty()) ImGui::TextColored(red_color, "Tile width must be a number!");
		if (!bTileHeightOk && !m_nTileHeight.empty()) ImGui::TextColored(red_color, "Tile height must be a number!");
		if (!bPatchValid && !m_LevelPath.empty()) ImGui::TextColored(red_color, "Path is invalid!");

		if (ImGui::Button("Create"))
		{
			if (bTileWidthOk && bTileHeightOk && bPatchValid)
			{ 
				const std::string level_path = m_LevelPath + "\\" + m_LevelName + ".json";
				const unsigned int tile_width = std::stoul(m_nTileWidth);
				const unsigned int tile_height = std::stoul(m_nTileHeight);

				m_Manager->CreateNewLevelRequest(level_path, m_LevelName, tile_width, tile_height);
			}
		}

		ImGui::End();
	}
}



/////////////////////////////////////////////////////////
void TilesetListWindow::Render()
{
		if (!m_Active)
			return;

		if (ImGui::Begin("Tilesets", &m_Active))
		{
			if (ImGui::Button("Add tileset"))
			{
				m_Manager->OpenAssetTableForAction(this);
			}


			ImGui::End();
		}
}

void TilesetListWindow::OnAssetSelected(const SelectedAssetData& data)
{
}

/////////////////////////////////////////////////////////
void 
