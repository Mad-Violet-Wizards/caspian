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
		}
	}
}