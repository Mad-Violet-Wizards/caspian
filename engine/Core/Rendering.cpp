#include "engine/pch.hpp"

#include "Rendering.hpp"

unsigned int Rendering::System::s_LevelTileSize = 0;
sf::Vector2f Rendering::System::s_RenderSize = { 1280.f, 720.f };
bool Rendering::System::s_RenderEmptyTiles = false;


void Rendering::System::Render(sf::RenderWindow& _window)
{
	if (ApplicationSingleton::Instance().GetWorld())
	{
		if (auto camera = ApplicationSingleton::Instance().GetWorld()->GetCamera())
		{
			_window.setView(camera->GetBaseView());

			if (GameObjectCollection* game_object_collection = ApplicationSingleton::Instance().GetEngineController().GetGameObjectStorage())
			{
				if (!game_object_collection->DrawablesEmpty())
				{
					const sf::Vector2f screen_size
					{
						static_cast<float>(sf::VideoMode::getDesktopMode().width),
						static_cast<float>(sf::VideoMode::getDesktopMode().height)
					};

					sf::FloatRect camera_rect{ camera->GetPosition().x - System::s_RenderSize.x,
																		 camera->GetPosition().y - System::s_RenderSize.y,
																		 screen_size.x,
																		 screen_size.y };

					if (camera_rect.left < 0) camera_rect.left = 0;
					if (camera_rect.top < 0) camera_rect.top = 0;

					const TileIndex top_left = game_object_collection->CalculateTileIndex(camera_rect.left, camera_rect.top);
					const TileIndex bottom_right = game_object_collection->CalculateTileIndex(camera_rect.left + camera_rect.width, camera_rect.top + camera_rect.height);
					SpatialHashGrid* spatial_hash_grid = game_object_collection->GetSpatialHashGrid();

					for (auto x = top_left.first; x < bottom_right.first; ++x)
					{
						for (auto y = top_left.second; y < bottom_right.second; ++y)
						{
							const TileIndex tile_index{ x, y };

							// Background tiles.
							for (auto it = spatial_hash_grid->GetRefToBucket(tile_index).background_objects_begin(); it != spatial_hash_grid->GetRefToBucket(tile_index).background_objects_end(); it++)
							{
								(*it)->GetComponent<C_Sprite>()->Draw(_window);
							}

							// Static entities.
							for (auto it = spatial_hash_grid->GetRefToBucket(tile_index).entity_objects_begin(); it != spatial_hash_grid->GetRefToBucket(tile_index).entity_objects_end(); it++)
							{
								(*it)->GetComponent<C_Sprite>()->Draw(_window);
							}

							// Dynamic entities.
							// Player / AIs
							// Projectiles

							// Foreground tiles
							for (auto it = spatial_hash_grid->GetRefToBucket(tile_index).foreground_objects_begin(); it != spatial_hash_grid->GetRefToBucket(tile_index).foreground_objects_end(); it++)
							{
								(*it)->GetComponent<C_Sprite>()->Draw(_window);
							}
						}

						// Debug Edit Collision Rendering - static level.
						if (ApplicationSingleton::Instance().GetEngineController().GetGameObjectStorage()->GetRenderCollidables())
						{
							const auto& collidable_go = ApplicationSingleton::Instance().GetEngineController().GetGameObjectStorage()->GetCollidableGameObjects();

							for (const auto& go : collidable_go)
							{
								if (auto c_sprite_sptr = go->GetComponent<C_Sprite>())
								{
									c_sprite_sptr->Draw(_window);
								}
							}
						}


						// Debug Collision Rendering - dynamic level.
					}
				}
			}
		}
	}
	//m_LevelRendering.Render(_window);
}