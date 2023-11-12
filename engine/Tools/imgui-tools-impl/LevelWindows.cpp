#include "engine/pch.hpp"

#include "LevelWindow.hpp"
#include "ToolsImpl.hpp"

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
		ImGui::InputText("Path", &m_LevelPath); // It has to be a dialog. At least I think so.
		ImGui::InputText("Layers", &m_nLayers);
		ImGui::InputText("Tile width", &m_nTileWidth);
		ImGui::InputText("Tile height", &m_nTileHeight);

		if (ImGui::Button("Create"))
		{
			if (m_LevelName.empty())
			{
				Tools::Log("Level name cannot be empty!");
				return;
			}

			if (m_LevelPath.empty())
			{
				Tools::Log("Level path cannot be empty!");
				return;
			}

			if (m_LevelWidth.empty())
			{
				Tools::Log("Level width cannot be empty!");
				return;
			}

			if (m_LevelHeight.empty())
			{
				Tools::Log("Level height cannot be empty!");
				return;
			}

			//Tools::CreateLevel(m_LevelName, m_LevelPath, std::stoi(m_LevelWidth), std::stoi(m_LevelHeight));
			m_Active = false;
		}
	}
}