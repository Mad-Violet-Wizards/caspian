#include "engine/pch.hpp"

#include "LevelWindows.hpp"
#include "ToolsImpl.hpp"
#include "engine/Core/Assets.hpp"
#include "engine/Core/Projects.hpp"

#include <imgui-file-dialog/ImGuiFileDialog.h>
#include <imgui-SFML/imgui-SFML.h>
#include <imgui/misc/cpp/imgui_stdlib.h>

using namespace Tools_Impl;

/////////////////////////////////////////////////////////
/* NewLevelWindow */
void NewLevelWindow::Render()
{
	if (ImGui::CollapsingHeader("New Level"))
	{
		ImGui::InputText("Name", &m_LevelName);
		ImGui::InputText("Path", &m_LevelPath); 
		ImGui::SameLine();
	
		constexpr auto dlg_name = "NewLevelWindowPathDialog";
		if (ImGui::Button("Select path"))
		{
			Projects::Manager* project_manager = ApplicationSingleton::Instance().GetProjectsManager();
			auto project_path = project_manager->GetCurrentProject().m_ProjectPath;
			project_path += "\\" + project_manager->GetCurrentProject().m_ProjectName + "\\";

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

		ImGui::InputText("Tile size", &m_TileSize);

		const bool bTileSizeOk = utils::FilterDigitOnly(m_TileSize);
		const bool bPatchValid = std::filesystem::exists(m_LevelPath);

		const ImVec4 red_color { 1.f, 0.f, 0.f, 1.f };

		if (!bTileSizeOk && !m_TileSize.empty()) ImGui::TextColored(red_color, "Tile size must be a number!");
		if (!bPatchValid && !m_LevelPath.empty()) ImGui::TextColored(red_color, "Path is invalid!");

		if (ImGui::Button("Create"))
		{
			if (bTileSizeOk && bPatchValid)
			{ 
				const std::string level_path = m_LevelPath + "\\" + m_LevelName + ".json";
				const unsigned int tile_size = std::stoul(m_TileSize);

				m_Manager->CreateNewLevelRequest(level_path, m_LevelName, tile_size);
			}
		}
	}
}

/////////////////////////////////////////////////////////
void LoadLevelWindow::Render()
{
	if (ImGui::CollapsingHeader("Load level"))
	{
		ImGui::Text("Select from cache");

		auto& main_instance = ApplicationSingleton::Instance();
		auto levels_data = main_instance.GetWorld()->GetInitialLevelsData();
		static std::optional<int> current_idx_levels;
		const std::string level_combo_preview_value = current_idx_levels.has_value() ? levels_data[current_idx_levels.value()].m_LevelName : "...";

		if (ImGui::BeginCombo("Cached levels", level_combo_preview_value.c_str()))
		{
			if (ImGui::Selectable("..."))
				current_idx_levels.reset();

			for (int i = 0; i < levels_data.size(); i++)
			{
				const bool is_selected = (current_idx_levels == i);
				if (ImGui::Selectable(levels_data[i].m_LevelName.c_str(), is_selected))
					current_idx_levels = i;

				// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}
		ImGui::SameLine();
		
		bool bComboSelected = current_idx_levels.has_value();
		static utils::styles styles;
		

		bComboSelected ? styles.push_accept_button_style() : styles.push_cancel_button_style();
		if (ImGui::Button("Load"))
		{
			if (bComboSelected)
			{
				main_instance.GetWorld()->SwitchToLevel(level_combo_preview_value);
			}
		}
		bComboSelected ? styles.pop_accept_button_style() : styles.pop_cancel_button_style();

		ImGui::Separator();
		ImGui::Text("...or select from disk");
		ImGui::Button("Select level");
	}
}

/////////////////////////////////////////////////////////
void TilesetListWindow::Update(float _dt)
{
	Assets::Storage* assets_storage = ApplicationSingleton::Instance().GetEngineController().GetAssetsStorage();
	const auto& tiles_vec = assets_storage->GetTilemapStorage()->GetTilesetsVec();

	// What's in tiles_vec should be in m_CachedTilesetRects
	for (const auto& tileset : tiles_vec)
	{
		Random::UUID tileset_uuid = tileset.m_TilesetUUID;

		if (m_CachedTilesetRects.find(tileset_uuid) == m_CachedTilesetRects.end())
		{
			const unsigned int tileset_width = assets_storage->GetTextureSize(tileset.m_TilesetPath).x;
			const unsigned int tileset_height = assets_storage->GetTextureSize(tileset.m_TilesetPath).y;
			const unsigned int tiles_in_column = tileset_height / tileset.m_TileHeight;
			const unsigned int tiles_in_row = tileset_width / tileset.m_TileWidth;

			std::vector<sf::IntRect> tileset_rects;

			for (auto y = 0; y < tiles_in_column; y++)
				for (auto x = 0; x < tiles_in_row; x++)
					tileset_rects.emplace_back(x * tileset.m_TileWidth, y * tileset.m_TileHeight, tileset.m_TileWidth, tileset.m_TileHeight);

			m_CachedTilesetRects[tileset_uuid] = tileset_rects;
		}
	}

	// What's not in tiles_vec should be erased from CachedTilesetRects
	for (auto it = m_CachedTilesetRects.begin(); it != m_CachedTilesetRects.end();)
	{
		const auto& tileset = std::find_if(tiles_vec.begin(), tiles_vec.end(), [it](const auto& _tileset) { return _tileset.m_TilesetUUID == it->first; });

		if (tileset == tiles_vec.end())
			it = m_CachedTilesetRects.erase(it);
		else
			it++;
	}
}

void TilesetListWindow::Render()
{
		if (ImGui::CollapsingHeader("Tilesets"))
		{
			if (ImGui::Button("Add tileset"))
			{
				m_Manager->OpenAssetTableForAction(this);
			}
			ImGui::SameLine();
			if (ImGui::Button("Tileset list"))
			{
				m_TilesetPreview = nullptr;
				m_bTilesetsListsOpened = true;
			}

			if (m_bTilesetsListsOpened)
			{
				static std::optional<int> current_idx;
				static std::string combo_preview_value;

				const auto& tileset_vec = ApplicationSingleton::Instance().GetEngineController().GetAssetsStorage()->GetTilemapStorage()->GetTilesetsVec();

				if (current_idx.has_value())
				{
					auto it = tileset_vec.begin();
					std::advance(it, current_idx.value());
					combo_preview_value = GetComboBoxTilesetName(it->m_TilesetUUID);
					m_PrevTilesetUUID = it->m_TilesetUUID;
				}
				else
				{
					combo_preview_value = "...";
				}

				int combo_current_idx = 0;
				if (ImGui::BeginCombo("Cached tilesets", combo_preview_value.c_str()))
				{
					if (ImGui::Selectable("..."))
						current_idx.reset();

					for (auto it = tileset_vec.begin(); it != tileset_vec.end(); it++, combo_current_idx++)
					{
						const bool is_selected = (current_idx == combo_current_idx);
						if (ImGui::Selectable(GetComboBoxTilesetName(it->m_TilesetUUID).c_str(), is_selected))
							current_idx = combo_current_idx;

						// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
						if (is_selected)
							ImGui::SetItemDefaultFocus();
					}
					ImGui::EndCombo();
				}

				if (current_idx.has_value())
				{
					Random::UUID selected_tileset_uuid = tileset_vec.at(current_idx.value()).m_TilesetUUID;
					const std::string& selected_tileset_path = tileset_vec.at(current_idx.value()).m_TilesetPath;
					const unsigned int selected_tileset_tile_width = tileset_vec.at(current_idx.value()).m_TileWidth;
					const unsigned int selected_tileset_tile_height = tileset_vec.at(current_idx.value()).m_TileHeight;
					Assets::Storage* assets_storage = ApplicationSingleton::Instance().GetEngineController().GetAssetsStorage();

					// Refresh m_Sprites only if selected tileset has changed.
					if (m_PrevTilesetUUID != selected_tileset_uuid)
					{
						m_CachedTilesetSprites.clear();

						const auto& tileset_rects = m_CachedTilesetRects.at(selected_tileset_uuid);

						for (const auto& rect : tileset_rects)
						{
							SelectedTileInfo tile_info;

							sf::Sprite sprite;
							sprite.setTexture(assets_storage->GetTexture(selected_tileset_path));
							sprite.setTextureRect(rect);

							tile_info.m_Sprite = sprite;
							m_CachedTilesetSprites.emplace_back(tile_info);
							m_PrevTilesetUUID = selected_tileset_uuid;
							m_CurrentTilesetHeight = assets_storage->GetTexture(selected_tileset_path).getSize().y;
						}
					}
					
					if (m_CachedTilesetSprites.size() > 0)
					{
						ImGui::BeginChild("TileSelector", ImVec2(0, 400), true, ImGuiWindowFlags_HorizontalScrollbar);

						const float availableWidth = ImGui::GetContentRegionAvail().x;
						const int columns = std::max(1, (int)(availableWidth / selected_tileset_tile_width));

						int counter = 0;
						bool b_img_button_is_selected = false;
						for (auto& tile_info : m_CachedTilesetSprites)
						{
							ImGui::PushID(&tile_info.m_Sprite);
							ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(selected_tileset_tile_width, selected_tileset_tile_width));

							tile_info.m_Selected ? ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.4f, 0.1f, 0.6f, 1.f)) : ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.f, 0.f, 0.f, 0.f));

							ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1.f, 0.7f, 0.1f, 1.f));
							ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.f, 0.5f, 0.f, 1.f));

							if (ImGui::ImageButton(tile_info.m_Sprite, ImVec2(selected_tileset_tile_width, selected_tileset_tile_height)))
							{
								if (m_SelectedTileInfo)
								{
									m_SelectedTileInfo->m_Selected = false;
									m_SelectedTileInfo = nullptr;
								}

								m_SelectedTileInfo = &tile_info;
								m_SelectedTileInfo->m_Selected = true;
							}

							ImGui::PopStyleVar();
							ImGui::PopStyleColor(3);
							ImGui::PopID();

							if (++counter % columns != 0)
								ImGui::SameLine();
						}

						ImGui::EndChild();
					}
				}
			}

			if (m_TilesetPreview != nullptr)
			{
				std::string fmt = std::format("Currently watching preview of: {}", m_QueuedToAddAsTileset->m_RelativePath);

				ImGui::InputText("Name", &m_TilesetName);
				ImGui::InputText("Tile Width", &m_nTileWidth);
				ImGui::InputText("Tile Height", &m_nTileHeight);

				const bool bTileWidthOk = utils::FilterDigitOnly(m_nTileWidth);
				const bool bTileHeightOk = utils::FilterDigitOnly(m_nTileHeight);

				const ImVec4 red_color{ 1.f, 0.f, 0.f, 1.f };

				if (!bTileWidthOk && !m_nTileHeight.empty()) ImGui::TextColored(red_color, "Tile width must be a number!");
				if (!bTileHeightOk && !m_nTileHeight.empty()) ImGui::TextColored(red_color, "Tile height must be a number!");

				ImGui::Separator();
				ImGui::Image(*m_TilesetPreview);
				ImGui::Separator();

				ImGui::Text(fmt.c_str());
				auto styles = utils::styles();

				styles.push_accept_button_style();
				if (ImGui::Button("Accept"))
				{
					if (bTileWidthOk && bTileHeightOk)
					{
						const unsigned int tile_width = std::stoul(m_nTileWidth);
						const unsigned int tile_height = std::stoul(m_nTileHeight);
						const std::string key{ m_QueuedToAddAsTileset->m_RelativePath };

						m_Manager->AddTilesetRequest(key, m_TilesetName, tile_width, tile_height);
					}
				}
				styles.pop_accept_button_style();

				ImGui::SameLine();
				styles.push_cancel_button_style();
				if (ImGui::Button("Cancel"))
				{
					m_TilesetPreview = nullptr;
				}
				styles.pop_cancel_button_style();
			}
		}
}

void TilesetListWindow::OnAssetSelected(const SelectedAssetData& data)
{
	m_QueuedToAddAsTileset = data;

	auto& engine_module = ApplicationSingleton::Instance().GetEngineController();
	const std::string key{ data.m_RelativePath };
	m_TilesetPreview = &engine_module.GetAssetsStorage()->GetConstTexture(key);
}

std::string TilesetListWindow::GetComboBoxTilesetName(Random::UUID _tileset_uuid) const
{
	const auto& assets_storage = ApplicationSingleton::Instance().GetEngineController().GetAssetsStorage();
	const auto& tilesets_vec = assets_storage->GetTilemapStorage()->GetTilesetsVec();
	
	for (auto& tileset : tilesets_vec)
	{
		if (tileset.m_TilesetUUID == _tileset_uuid)
		{
			return std::format("{} ({})", tileset.m_TilesetName, tileset.m_TilesetPath);
		}
	}

	return "Unknown";
}

/////////////////////////////////////////////////////////
void LevelEditorWindow::Update(float _dt)
{
	if (!m_Active)
		return;

	m_NewLevelWindow.Update(_dt);
	m_LoadLevelWindow.Update(_dt);
	m_TilesetListWindow.Update(_dt);
}

void LevelEditorWindow::Render()
{
	if (!m_Active)
		return;

	if (ImGui::Begin("Level Editor", &m_Active))
	{
		m_NewLevelWindow.Render();
		ImGui::Dummy({ 0.f, 10.f});
		m_LoadLevelWindow.Render();
		ImGui::Dummy({ 0.f, 10.f });
		m_TilesetListWindow.Render();

		ImGui::End();
	}
}