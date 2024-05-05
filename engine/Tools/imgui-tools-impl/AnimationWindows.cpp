#include "engine/pch.hpp"

#include "AnimationWindows.hpp"
#include "ToolsImpl.hpp"

#include <imgui-SFML/imgui-SFML.h>
#include <imgui/misc/cpp/imgui_stdlib.h>

using namespace Tools_Impl;


void AnimationPreviewWindow::Render()
{
	if (ImGui::CollapsingHeader("Animation Preview"))
	{
		if (m_AnimationTexturePreview == nullptr)
		{
			ImGui::Text("No animation tileset found. Please select one in the 'New Animation' or 'Edit Animation' window.");
		}
		else
		{
			for (AnimationFramePreviewInternalData& frame_data : m_AnimationFrameData)
			{
				ImGui::Text(frame_data.m_Label.c_str());

				sf::Sprite sprite;
				sprite.setTexture(*m_AnimationTexturePreview);
				sprite.setTextureRect(frame_data.m_Rect);
				
				ImGui::Image(sprite);
			}
		}
	}
}

void AnimationPreviewWindow::Update(float _dt)
{

}

void AnimationPreviewWindow::SetTexture(const std::string& _texture_path)
{
	Assets::Storage* assets_storage = ApplicationSingleton::Instance().GetEngineController().GetAssetsStorage();
	m_AnimationTexturePreview = &assets_storage->GetConstTexture(_texture_path);
}

void AnimationPreviewWindow::SyncNewAnimationData(const std::vector<AnimationFrameInternalData>& _frame_internal_data)
{
	m_AnimationFrameData.clear();

	int idx = 1;
	for (const AnimationFrameInternalData& frame_data : _frame_internal_data)
	{
		if (utils::FilterDigitOnly(frame_data.m_FrameTime)
			&& utils::FilterDigitOnly(frame_data.m_FrameRectX)
			&& utils::FilterDigitOnly(frame_data.m_FrameRectY)
			&& utils::FilterDigitOnly(frame_data.m_FrameRectWidth)
			&& utils::FilterDigitOnly(frame_data.m_FrameRectHeight)
			)
		{
			AnimationFramePreviewInternalData preview_data;
			preview_data.m_Label = "Frame " + std::to_string(idx);
			preview_data.m_FrameTime = std::stof(frame_data.m_FrameTime);
			preview_data.m_Rect.left = std::stoi(frame_data.m_FrameRectX);
			preview_data.m_Rect.top = std::stoi(frame_data.m_FrameRectY);
			preview_data.m_Rect.width = std::stoi(frame_data.m_FrameRectWidth);
			preview_data.m_Rect.height = std::stoi(frame_data.m_FrameRectHeight);

			m_AnimationFrameData.push_back(preview_data);

			idx++;
		}
	}
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
		ImGui::InputText("Animation name: ", &m_AnimationName);
		ImGui::InputText("Animation frames count: ", &m_AnimationFrameCount);
		
		if (ImGui::Button("Select animation tileset"))
		{
			m_Manager->OpenAssetTableForAction(this);
		}

		const bool bFrameCountOk = utils::FilterDigitOnly(m_AnimationFrameCount);

		if (bFrameCountOk)
		{
			unsigned int frame_count = std::stoul(m_AnimationFrameCount);

			if (frame_count != m_PrevAnimationFrameCountInt)
			{
				m_AnimationData.clear();
				m_PrevAnimationFrameCountInt = frame_count;
				m_AnimationData.resize(frame_count);
			}

			for (unsigned int i = 0; i < frame_count; ++i)
			{
				ImGui::PushID(i);
				ImGui::Text("Frame %d", i);
				ImGui::InputText("Frame time (ms): ", &m_AnimationData[i].m_FrameTime);
				ImGui::InputText("x: ", &m_AnimationData[i].m_FrameRectX);
				ImGui::InputText("y: ", &m_AnimationData[i].m_FrameRectY);
				ImGui::InputText("width: ", &m_AnimationData[i].m_FrameRectWidth);
				ImGui::InputText("height: ", &m_AnimationData[i].m_FrameRectHeight);
				ImGui::PopID();
			}
		}
	}
}

void NewAnimationWindow::Update(float _dt)
{
	const bool bFrameCountOk = utils::FilterDigitOnly(m_AnimationFrameCount);

	if (bFrameCountOk)
	{
		m_PreviewWindow->SyncNewAnimationData(m_AnimationData);
	}
}

void NewAnimationWindow::OnAssetSelected(const SelectedAssetData& data)
{
	m_PreviewWindow->SetTexture(*data.m_RelativePath);
	m_AnimationTexturePath = *data.m_RelativePath;
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

