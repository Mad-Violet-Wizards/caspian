#include "engine/pch.hpp"

#include "Rendering.hpp"

unsigned int Rendering::System::s_TileSize = 0;

void Rendering::System::Render(sf::RenderWindow& _window)
{

	if (ApplicationSingleton::Instance().GetWorld())
	{
		if (auto camera = ApplicationSingleton::Instance().GetWorld()->GetCamera())
		{
			_window.setView(camera->GetBaseView());
		}
	}

	m_LevelRendering.Render(_window);
}

void Rendering::System::OnLevelActivated(Level::Level* _level)
{
	const Level::ChunksManager* chunk_manager = _level->GetChunksManager();

	s_TileSize = _level->GetTilesSize();

	for (const auto& chunk : chunk_manager->GetChunks())
	{
		ProcessChunk(chunk.get());
	}
}

void Rendering::System::ProcessChunk(Level::Chunk* _level_chunk)
{
	const auto& chunk_info = _level_chunk->GetChunkInfo();

	if (m_LevelRendering.ContainsChunk(chunk_info->m_ChunkUUID))
	{
		return;
	}

	Assets::Storage* assets_storage = ApplicationSingleton::Instance().GetEngineController().GetAssetsStorage();

	RenderChunk render_chunk(chunk_info->m_ChunkUUID);

	for (const Serializable::Binary::TextureLayerInfo& tile_layer_info : chunk_info->m_TileLayers)
	{
		Rendering::TextureLayer texture_layer(tile_layer_info.m_Tiles.size());

		for (auto x = _level_chunk->GetArea().left; x < _level_chunk->GetArea().width; x += Rendering::System::s_TileSize)
		{
			for (auto y = _level_chunk->GetArea().top; y < _level_chunk->GetArea().height; y += Rendering::System::s_TileSize)
			{
				sf::Sprite* sprite = new sf::Sprite();
				sprite->setTextureRect(sf::IntRect(0, 0, 32, 32));
				sprite->setTexture(assets_storage->GetEmptyTexture());
				sprite->setPosition(x, y);
				texture_layer.AddSprite(sprite);
			}
		}

		m_LevelRendering.AddToSpatialHash(texture_layer);

		/*for (const Serializable::Binary::TextureTileInfo& tile_info : tile_layer_info.m_Tiles)
		{
			if (tile_info.m_TilesetUUID == Random::EMPTY_UUID)
			{
				sf::Sprite* sprite = new sf::Sprite();
				sprite->setTextureRect(sf::IntRect(0, 0, 32, 32));
				sprite->setTexture(assets_storage->GetEmptyTexture());
				texture_layer.AddSprite(sprite);
				continue;
			}

			const auto& tileset = assets_storage->GetTilemapStorage()->FindTilesetInfo(tile_info.m_TilesetUUID);
		}*/
		
		render_chunk.AddTextureLayer(texture_layer);
	}

	m_LevelRendering.AddChunk(render_chunk);
}

void Rendering::LevelRendering::Render(sf::RenderWindow& _window)
{
	auto camera = ApplicationSingleton::Instance().GetWorld()->GetCamera();

	const sf::View& camera_base_view = camera->GetBaseView();

	const sf::Vector2f screen_size_offset
	{ 
		static_cast<float>(sf::VideoMode::getDesktopMode().width) * 0.5f,
		static_cast<float>(sf::VideoMode::getDesktopMode().height) * 0.5f 
	};

	sf::FloatRect camera_rect{ camera->GetPosition().x - screen_size_offset.x, 
														 camera->GetPosition().y - screen_size_offset.y, 
														 camera_base_view.getSize().x + screen_size_offset.x, 
													   camera_base_view.getSize().y + screen_size_offset.y };

	if (camera_rect.left < 0) camera_rect.left = 0;
	if (camera_rect.top < 0) camera_rect.top = 0;

	TileIndex top_left = m_SpatialHashGrid.GetTileIndex({ camera_rect.left, camera_rect.top });
	TileIndex bottom_right = m_SpatialHashGrid.GetTileIndex({ camera_rect.left + camera_rect.width, camera_rect.top + camera_rect.height });

	int hit_count = 0;

	if (m_SpatialHashGrid.GetSize() > 0)
	{
		for (auto x = top_left.first; x < bottom_right.first; x++)
		{
			for (auto y = top_left.second; y < bottom_right.second; y++)
			{
					const auto& sprites = m_SpatialHashGrid.FindSpatialBucket({ x, y });

					for (const auto& sprite : sprites)
					{
						hit_count++;
						if (sprite)
							_window.draw(*sprite);
					}
			}
		}
	}

	std::cout << "Hit count: " << hit_count << std::endl;
}

bool Rendering::LevelRendering::ContainsChunk(Random::UUID _chunk_uuid) const
{
	for (const auto& render_chunk : m_RenderChunks)
	{
		if (render_chunk.GetUUID() == _chunk_uuid)
		{
			return true;
		}
	}

	return false;
}


void Rendering::LevelRendering::AddToSpatialHash(const TextureLayer& _texture_layer)
{
	m_SpatialHashGrid.ProcessTextureTileLayer(_texture_layer);
}

Rendering::RenderChunk::RenderChunk(Random::UUID _owning_chunk_uuid)
	: m_OwningChunkUUID(_owning_chunk_uuid) {}

Rendering::TextureLayer::TextureLayer(unsigned int _size)
	: m_LayerIndex(-1)
{
	m_CachedSprites.reserve(_size);
}

void Rendering::TextureLayer::AddSprite(sf::Sprite* _sprite)
{
	m_CachedSprites.push_back(_sprite);
}

void Rendering::SpatialHashGrid::ProcessTextureTileLayer(const TextureLayer& _texture_layer)
{
	for (auto& sprite : _texture_layer.GetSprites())
	{
		const TileIndex& tile_index = GetTileIndex(sprite->getPosition());
		m_SpatialHash[tile_index].push_back(sprite);
	}
}

const std::vector<sf::Sprite*>& Rendering::SpatialHashGrid::FindSpatialBucket(const TileIndex& _index) const
{
	auto it = m_SpatialHash.find(_index);

	if (it != m_SpatialHash.end())
	{
		return it->second;
	}
}

Rendering::TileIndex Rendering::SpatialHashGrid::GetTileIndex(const sf::Vector2f& _pos) const
{
	return { _pos.x / SPATIAL_BUCKET_SIZE, _pos.y / SPATIAL_BUCKET_SIZE };
}

Rendering::RenderTile::RenderTile(const sf::Vector2u& _world_position, const sf::IntRect& _texture_rect, Random::UUID _tileset_uuid)
	: m_WorldPosition(_world_position)
	, m_TextureRect(_texture_rect)
	, m_TilesetUUID(_tileset_uuid)
{

}
