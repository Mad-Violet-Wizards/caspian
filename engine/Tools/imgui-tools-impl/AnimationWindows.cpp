#include "engine/pch.hpp"

#include "AnimationWindows.hpp"
#include "ToolsImpl.hpp"

#include <imgui/misc/cpp/imgui_stdlib.h>

using namespace Tools_Impl;


void AnimationPreviewWindow::Render()
{
	if (ImGui::CollapsingHeader("Animation Preview"))
	{
		ImGui::Text("Anim preview window");
	}
}

void AnimationPreviewWindow::Update(float _dt)
{

}

void AnimationListWindow::Render()
{
	if (ImGui::CollapsingHeader("Animations List"))
	{

	}
}

void AnimationListWindow::Update(float _dt)
{

}

void NewAnimationWindow::Render()
{
	if (ImGui::CollapsingHeader("New Animation"))
	{
		//ImGui::InputText("Animation name: ", &m_AnimationName);
		//ImGui::InputText("Animation frames count: ", &m_AnimationFrameCount);
		//ImGui::InputText("")


	}
}

void NewAnimationWindow::Update(float _dt)
{

}

void EditAnimationWindow::Render()
{
	if (ImGui::CollapsingHeader("Edit Animation"))
	{

	}
}

void EditAnimationWindow::Update(float _dt)
{

}

void AnimationEditortWindow::Render()
{
	if (!m_Active)
		return;

	if (ImGui::Begin("Animations Editor", &m_Active))
	{
		m_AnimationPreviewWindow.Render();
		ImGui::Dummy({ 0.f, 10.f });
		m_AnimationListWindow.Render();
		ImGui::Dummy({ 0.f, 10.f });
		m_EditAnimationWindow.Render();
		ImGui::Dummy({ 0.f, 10.f });
		m_NewAnimationWindow.Render();

		ImGui::End();
	}
}

void AnimationEditortWindow::Update(float _dt)
{
		if (!m_Active)
		return;

	m_AnimationPreviewWindow.Update(_dt);
	m_AnimationListWindow.Update(_dt);
	m_EditAnimationWindow.Update(_dt);
	m_NewAnimationWindow.Update(_dt);
}

