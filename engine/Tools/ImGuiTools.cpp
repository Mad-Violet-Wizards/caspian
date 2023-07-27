#include "engine/pch.hpp"

#include <imgui/imgui.h>
#include "engine/Tools/ImGuiTools.hpp"

using namespace Tools;

////////////////////////////////////////////////////////
/* ImGuiMenubar */
void ImGuiMenubar::Update(float _dt)
{

}

void ImGuiMenubar::Render()
{

}

////////////////////////////////////////////////////////
/* ImGuiManager */
void ImGuiManager::ToggleActiveState()
{
	m_Active = !m_Active;
}

void ImGuiManager::Update(float _dt)
{
	if (!m_Active)
		return;
}

void ImGuiManager::Render()
{
	if (!m_Active)
		return;
	
	{
		ImGui::Begin("Hello, world!");
		ImGui::Button("Some debug button.");
		ImGui::End();
	}
}
