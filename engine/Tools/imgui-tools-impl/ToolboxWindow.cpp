#include "engine/pch.hpp"

#include "ToolboxWindow.hpp"

using namespace Tools_Impl;

/////////////////////////////////////////////////////////
ToolboxWindow::ToolboxWindow(Manager* _mgr)
	: IWindow(_mgr)
	, m_CameraToolbox(_mgr)
	, m_LevelToolbox(_mgr)
	, m_FpsCountLimit(120)
{
	m_SavedFps.reserve(m_FpsCountLimit);
}

void ToolboxWindow::Update(float _dt)
{
	if (!m_Active)
		return;

	m_CameraToolbox.Update(_dt);
}

void ToolboxWindow::Render()
{
	if (!m_Active)
		return;

	ImGui::Begin("Toolbox", &m_Active);
	{
		if (ImGui::CollapsingHeader("Debug info"))
		{
			auto& main_instance = ApplicationSingleton::Instance();

			static int frame_count = 0;

			if (frame_count < m_FpsCountLimit)
			{
				m_SavedFps.push_back(1.f / main_instance.GetDeltaTime());
				frame_count++;
			}
			else
			{
				m_Fps = *std::min_element(m_SavedFps.begin(), m_SavedFps.end());
				frame_count = 0;
				m_SavedFps.clear();
				m_SavedFps.reserve(m_FpsCountLimit);

			}
			const std::string fps_text = std::format("FPS: {}", m_Fps);
			ImGui::Text(fps_text.c_str());
		}

		ImGui::Dummy(ImVec2(0, 10));
		m_CameraToolbox.Render();
		ImGui::Dummy(ImVec2(0, 10));
		m_LevelToolbox.Render();

		ImGui::End();
	}
}

/////////////////////////////////////////////////////////
void CameraToolbox::Update(float _dt)
{
}

void CameraToolbox::Render()
{
	if (ImGui::CollapsingHeader("Camera Toolbox"))
	{
		auto& main_instance = ApplicationSingleton::Instance();

		if (main_instance.GetWorld())
		{
			if (auto camera = main_instance.GetWorld()->GetCamera())
			{
				const sf::Vector2f& camera_pos = camera->GetPosition();
				const std::string camera_pos_text = std::format("Camera position (center): (x: {}, y: {})", camera_pos.x, camera_pos.y);
				ImGui::Text(camera_pos_text.c_str());
			}
		}

		m_MoveCameraButton.Render();
	}
}

CameraToolbox::CameraToolbox(Manager* _mgr)
	: IWindow(_mgr)
	, m_MoveCameraButton("Move Camera", std::bind(&CameraToolbox::OnMoveCameraToggled, this), false)
{

}

void CameraToolbox::OnMoveCameraToggled()
{
	auto& main_instance = ApplicationSingleton::Instance();
	main_instance.GetEditControllers().GetCameraController()->SetActive(m_MoveCameraButton.IsActive());

}

/////////////////////////////////////////////////////////
LevelToolbox::LevelToolbox(Manager* _mgr)
	: IWindow(_mgr)
	, m_PlaceTileButton("Place Tile", std::bind(&LevelToolbox::OnPlaceTileToggled, this), false)
	, m_EraseTileButton("Erase Tile", std::bind(&LevelToolbox::OnEraseTileToggled, this), false)
	, m_EditCollisionTileButton("Edit Collisions", std::bind(&LevelToolbox::OnPlaceCollisionTileToggled, this), false)
{

}

void LevelToolbox::Update(float _dt)
{
}

void LevelToolbox::Render()
{
	if (ImGui::CollapsingHeader("Level Toolbox"))
	{
		m_PlaceTileButton.Render();
		ImGui::SameLine();
		m_EraseTileButton.Render();
		ImGui::SameLine();
		m_EditCollisionTileButton.Render();

		auto& main_instance = ApplicationSingleton::Instance();

		Levels::World* world = main_instance.GetWorld();

		if (world->IsLevelActive())
		{
			const LevelEditController::WorkingLayer& current_layer = main_instance.GetEditControllers().GetLevelController()->GetCurrentSelectedWorkingLayer();

			ImGui::Text("Current working layer: ");
			ImGui::SameLine();
			const std::string layer_text = current_layer.m_Tag == ETag::None ? "None" : std::format("Layer {} ({})", current_layer.m_LayerIndex, current_layer.m_Tag == ETag::Drawable_Background ? "Background" : "Foreground");
			ImGui::Text(layer_text.c_str());

			const unsigned int background_layers = world->GetActiveLevel()->GetNoBackroundLayers();

			if (background_layers != 0)
			{
				for (auto i = 0; i < background_layers; i++)
				{
					const std::string layer_text = std::format("Background Layer {}", i);
					if (ImGui::Selectable(layer_text.c_str()))
					{
						main_instance.GetEditControllers().GetLevelController()->SetSelectedWorkingLayer(i, ETag::Drawable_Background);
					}
				}
			}

			const unsigned int foreground_layers = world->GetActiveLevel()->GetNoForegroundLayers();
			if (foreground_layers != 0)
			{
				for (auto i = 0; i < foreground_layers; i++)
				{
					const std::string layer_text = std::format("Foreground Layer {}", i);
					if (ImGui::Selectable(layer_text.c_str()))
					{
						main_instance.GetEditControllers().GetLevelController()->SetSelectedWorkingLayer(i, ETag::Drawable_Foreground);
					}
				}
			}
		}
	}
}

void LevelToolbox::OnPlaceTileToggled()
{
	auto& main_instance = ApplicationSingleton::Instance();

	const bool is_active = m_PlaceTileButton.IsActive();
	main_instance.GetEditControllers().GetLevelController()->SetActive(is_active);

	if (is_active)
	{
		m_EraseTileButton.m_Active = false;
		m_EditCollisionTileButton.m_Active = false;
		main_instance.GetEditControllers().GetLevelController()->SetMode(ELevelEditControllerMode::PaintTile);
	}
	else
	{
		main_instance.GetEditControllers().GetLevelController()->SetMode(ELevelEditControllerMode::None);
	}
}

void LevelToolbox::OnEraseTileToggled()
{
	auto& main_instance = ApplicationSingleton::Instance();

	const bool is_active = m_EraseTileButton.IsActive();
	main_instance.GetEditControllers().GetLevelController()->SetActive(is_active);

	if (is_active)
	{
		main_instance.GetEditControllers().GetLevelController()->SetMode(ELevelEditControllerMode::Erase);
		m_PlaceTileButton.m_Active = false;
		m_EditCollisionTileButton.m_Active = false;
	}
	else
	{
		main_instance.GetEditControllers().GetLevelController()->SetMode(ELevelEditControllerMode::None);
	}
}

void LevelToolbox::OnPlaceCollisionTileToggled()
{
	auto& main_instance = ApplicationSingleton::Instance();

	if (main_instance.GetWorld()->GetActiveLevel() == nullptr)
	{
		m_Manager->ShowNotification(ENotificationType::Error, "You must activate level first.");
		m_EditCollisionTileButton.m_Active = false;	
		return;
	}
	
	const bool is_active = m_EditCollisionTileButton.IsActive();
	main_instance.GetEditControllers().GetLevelController()->SetActive(is_active);

	if (is_active)
	{
		main_instance.GetEditControllers().GetLevelController()->SetMode(ELevelEditControllerMode::EditCollisions);
		m_EraseTileButton.m_Active = false;
		m_PlaceTileButton.m_Active = false;
	}
	else
	{
		main_instance.GetEditControllers().GetLevelController()->SetMode(ELevelEditControllerMode::None);
	}
}

