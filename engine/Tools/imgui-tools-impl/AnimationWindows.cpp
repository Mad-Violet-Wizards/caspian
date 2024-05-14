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
		if (m_AnimationTexturePreview == nullptr || m_AnimationFrameData.empty() && !m_Playing)
		{
			ImGui::Text("No animation tileset found or animation data missing.\n"
								   "Please select one in the 'New Animation' or 'Edit Animation' window.");
		}
		else
		{
			ImGui::Text("Animation preview:");
			sf::Sprite sprite;
			sprite.setScale(m_AnimationScalesArray[m_CurrentScalesArrayIndex].second, m_AnimationScalesArray[m_CurrentScalesArrayIndex].second);
			sprite.setTexture(*m_AnimationTexturePreview);
			sprite.setTextureRect(m_AnimationFrameData[m_CurrentAnimationFrameIndex].m_Rect);
			ImGui::Image(sprite);

			m_PlayButton.Render();
			ImGui::SameLine();
			m_StopButton.Render();
			ImGui::SameLine();
			m_PauseButton.Render();
			ImGui::SameLine();
			const std::string frame_time = std::format("{:.2f}{}", m_CurrentFrameTime, "ms");
			ImGui::Text(frame_time.c_str());

			const std::string current_frame = std::format("Frame: {}", m_CurrentAnimationFrameIndex);
			if (ImGui::BeginCombo("Select frame", current_frame.c_str()))
			{
				for (auto i = 0; i < m_AnimationFrameData.size(); i++)
				{
					bool is_selected = (m_CurrentAnimationFrameIndex == i);
					if (ImGui::Selectable(m_AnimationFrameData[i].m_Label.c_str(), is_selected))
					{
						m_CurrentAnimationFrameIndex = i;
						m_CurrentFrameTime = 0.0f;
					}
					if (is_selected)
					{
						ImGui::SetItemDefaultFocus();
					}
				}
				ImGui::EndCombo();
			}

			if (ImGui::BeginCombo("Scale", m_AnimationScalesArray[m_CurrentScalesArrayIndex].first.c_str()))
			{
				for (auto i = 0; i < m_AnimationScalesArray.size(); i++)
				{
					bool is_selected = (m_CurrentScalesArrayIndex == i);
					if (ImGui::Selectable(m_AnimationScalesArray[i].first.c_str(), is_selected))
					{
						m_CurrentScalesArrayIndex = i;
					}
					if (is_selected)
					{
						ImGui::SetItemDefaultFocus();
					}
				}

				ImGui::EndCombo();
			}


			if (ImGui::BeginCombo("Slowdown", m_AnimationSpeedValuesArray[m_CurrentSpeedValuesArrayIndex].first.c_str()))
			{
				for (auto i = 0; i < m_AnimationSpeedValuesArray.size(); i++)
				{
					bool is_selected = (m_CurrentSpeedValuesArrayIndex == i);

					if (ImGui::Selectable(m_AnimationSpeedValuesArray[i].first.c_str(), is_selected))
					{
						m_CurrentSpeedValuesArrayIndex = i;
					}
					if (is_selected)
					{
						ImGui::SetItemDefaultFocus();
					}
				}

				ImGui::EndCombo();
			}
		}
	}
}

void AnimationPreviewWindow::Update(float _dt)
{
	if (m_AnimationTexturePreview && !m_AnimationFrameData.empty() && m_Playing)
	{
		m_CurrentFrameTime += _dt * 1000.f * m_AnimationSpeedValuesArray[m_CurrentSpeedValuesArrayIndex].second;

		if (m_CurrentFrameTime >= m_AnimationFrameData[m_CurrentAnimationFrameIndex].m_FrameTime)
		{
			m_CurrentFrameTime = 0.0f;
			m_CurrentAnimationFrameIndex++;
			if (m_CurrentAnimationFrameIndex >= m_AnimationFrameData.size())
			{
				m_CurrentAnimationFrameIndex = 0;
				m_CurrentFrameTime = 0.f;
				OnAnimationFinished();
			}
		}
	}
}

void AnimationPreviewWindow::SetTexture(const std::string& _texture_path)
{
	Assets::Storage* assets_storage = ApplicationSingleton::Instance().GetEngineController().GetAssetsStorage();
	m_AnimationTexturePreview = &assets_storage->GetConstTexture(_texture_path);
}

void AnimationPreviewWindow::CreatePreviewDataFromFrameInternals(const std::vector<AnimationFrameInternalData>& _frame_internal_data)
{
	m_AnimationFrameData.clear();

	int idx = 0;
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

void AnimationPreviewWindow::OnPlayButtonToggled()
{
	m_PlayButton.m_Active = true;
	m_PauseButton.m_Active = false;
	m_Playing = true;
}

void AnimationPreviewWindow::OnStopButtonToggled()
{
	m_PlayButton.m_Active = false;
	m_PauseButton.m_Active = false;
	m_StopButton.m_Active = false;
	m_Playing = false;
	m_CurrentAnimationFrameIndex = 0;
	m_CurrentFrameTime = 0.0f;
}

void AnimationPreviewWindow::OnPauseButtonToggled()
{
	m_PlayButton.m_Active = false;
	m_Playing = false;
}

void AnimationPreviewWindow::OnAnimationFinished()
{
	switch (m_CurrentAnimationType)
	{
		case EAnimationType::Loop:
		{
			// Do nothing.
		}
		break;
		case EAnimationType::OneShot:
		{
			OnStopButtonToggled();
		}
		break;
		case EAnimationType::PingPong:
		{
			std::reverse(m_AnimationFrameData.begin(), m_AnimationFrameData.end());
		}
		break;
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

void AnimationPropertiesWindow::Render()
{
	if (ImGui::TreeNode("Animation Properties"))
	{
		ImGui::InputText("Animation name: ", &m_AnimationName);
		ImGui::InputText("Animation frames count: ", &m_AnimationFrameCount);

		const std::string current_anim_type = std::format("Animation type: {}", ToString(m_AnimationType));
		if (ImGui::BeginCombo("Select animation type", current_anim_type.c_str()))
		{
			for (auto i = 0; i < m_AnimationTypes.size(); i++)
			{
				bool is_selected = (m_AnimationType == m_AnimationTypes[i]);
				if (ImGui::Selectable(ToString(m_AnimationTypes[i]), is_selected))
				{
					m_AnimationType = m_AnimationTypes[i];
				}
				if (is_selected)
				{
					ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndCombo();
		}

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

		ImGui::TreePop();
	}
}

void AnimationPropertiesWindow::Update(float _dt)
{
}

void AnimationPropertiesWindow::OnAssetSelected(const SelectedAssetData& data)
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

void AnimationEditorWindow::Render()
{
	if (!m_Active)
		return;

	if (ImGui::Begin("Animations Editor", &m_Active))
	{
		m_AnimationPreviewWindow.Render();
		ImGui::Separator();
		ImGui::Dummy({ 0.f, 10.f });
		m_AnimationListWindow.Render();
		ImGui::Separator();
		ImGui::Dummy({ 0.f, 10.f });
		m_EditAnimationWindow.Render();
		ImGui::Separator();
		ImGui::Dummy({ 0.f, 10.f });
		m_NewAnimationWindow.Render();

		ImGui::End();
	}
}

void AnimationEditorWindow::Update(float _dt)
{
		if (!m_Active)
		return;

	m_AnimationPreviewWindow.Update(_dt);
	m_AnimationListWindow.Update(_dt);
	m_EditAnimationWindow.Update(_dt);
	m_NewAnimationWindow.Update(_dt);
}

void NewAnimationWindow::Render()
{
	if (ImGui::CollapsingHeader("New animation"))
	{
		m_AnimationPropertiesWindow.Render();
		m_PlayInPreview.Render();
	}
}

void NewAnimationWindow::Update(float _dt)
{

}

void NewAnimationWindow::OnPlayInPreviewButtonToggled()
{
	m_PlayInPreview.m_Active = false;
	m_PreviewWindow->SetAnimationPreviewOwner(EAnimationPreviewOwner::NewAnimWindow);
	m_PreviewWindow->CreatePreviewDataFromFrameInternals(m_AnimationPropertiesWindow.GetAnimationData());
	m_PreviewWindow->SetAnimationType(m_AnimationPropertiesWindow.GetAnimationType());
}
