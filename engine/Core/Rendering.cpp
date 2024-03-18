#include "engine/pch.hpp"

#include "Rendering.hpp"

unsigned int Rendering::System::s_LevelTileSize = 0;
sf::Vector2f Rendering::System::s_RenderSize = { 1280.f, 720.f };

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

	s_LevelTileSize = _level->GetTilesSize();

	for (const auto& chunk : chunk_manager->GetChunks())
		ProcessLevelChunk(chunk.get());
}

void Rendering::System::OnLevelDeactivated(Level::Level* _level)
{
	m_LevelRendering.Clear();
}

void Rendering::System::ProcessLevelChunk(Level::Chunk* _level_chunk)
{
	const auto& chunk_info = _level_chunk->GetChunkInfo();

	m_LevelRendering.MarkChunk(chunk_info->m_ChunkUUID);

	for (auto reverse_it = chunk_info->m_TileLayers.rbegin(); reverse_it != chunk_info->m_TileLayers.rend(); ++reverse_it)
	{
		const Serializable::Binary::TextureLayerInfo& tile_layer_info = *reverse_it;

		for (const auto& _tile_info : tile_layer_info.m_Tiles)
		{
			const sf::Vector2u tile_world_pos { _tile_info.m_TilePositionX, _tile_info.m_TilePositionY };
			const sf::Vector2u tile_position_in_tileset { _tile_info.m_TilesetRow, _tile_info.m_TilesetColumn };

			Rendering::RenderTile* tile = new Rendering::RenderTile(tile_layer_info.m_LayerIndex, tile_world_pos, tile_position_in_tileset, _tile_info.m_TilesetUUID);

			m_LevelRendering.GetSpatialHashGrid().ProcessRenderTile(tile);
		}
	}
}

void Rendering::LevelRendering::Render(sf::RenderWindow& _window)
{
	if (m_SpatialHashGrid.GetSize() == 0)
		return;

	const auto& camera = ApplicationSingleton::Instance().GetWorld()->GetCamera();

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

	const TileIndex top_left = m_SpatialHashGrid.GetTileIndexVecFloat({ camera_rect.left, camera_rect.top });
	const TileIndex bottom_right = m_SpatialHashGrid.GetTileIndexVecFloat({ camera_rect.left + camera_rect.width, camera_rect.top + camera_rect.height });

	for (auto x = top_left.first; x < bottom_right.first; ++x)
	{
		for (auto y = top_left.second; y < bottom_right.second; ++y)
		{
			const std::vector<RenderTile*>& tiles = m_SpatialHashGrid.GetTiles({ x, y });

			for (RenderTile* tile : tiles)
				_window.draw(tile->GetSprite());
		}
	}
}

void Rendering::LevelRendering::MarkChunk(Random::UUID _chunk_uuid)
{
	m_CurrentChunks.push_back(_chunk_uuid);
}

bool Rendering::LevelRendering::ContainsChunk(Random::UUID _chunk_uuid) const
{
	for (const Random::UUID chunk : m_CurrentChunks)
	{
		if (chunk == _chunk_uuid)
			return true;
	}

	return false;
}

void Rendering::LevelRendering::Clear()
{
	m_CurrentChunks.clear();
	m_SpatialHashGrid.Clear();
}

Rendering::SpatialHashGrid::~SpatialHashGrid()
{
	for (auto& [key, value] : m_SpatialHash)
	{
		for (auto& tile : value)
			delete tile;
		value.clear();
	}

	m_SpatialHash.clear();
}

void Rendering::SpatialHashGrid::ProcessRenderTile(RenderTile* _render_tile)
{
	const TileIndex chimney_idx = { _render_tile->GetWorldPosition().x, _render_tile->GetWorldPosition().y };

	if (m_Chimneys.find(chimney_idx) == m_Chimneys.end() || m_Chimneys[chimney_idx] == false)
	{
		const TileIndex tile_index = GetTileIndex(_render_tile->GetWorldPosition());

		m_SpatialHash[tile_index].push_back(_render_tile);
	}
	else
	{
		delete _render_tile;
	}
}

Rendering::TileIndex Rendering::SpatialHashGrid::GetTileIndex(const sf::Vector2u& _pos) const
{
	return { _pos.x / SPATIAL_BUCKET_SIZE, _pos.y / SPATIAL_BUCKET_SIZE };
}

Rendering::TileIndex Rendering::SpatialHashGrid::GetTileIndexVecFloat(const sf::Vector2f& _pos) const
{
	return { _pos.x / SPATIAL_BUCKET_SIZE, _pos.y / SPATIAL_BUCKET_SIZE };
}

void Rendering::SpatialHashGrid::Clear()
{
	m_SpatialHash.clear();
}

Rendering::RenderTile::RenderTile(int _layer_index, const sf::Vector2u& _world_position, const sf::Vector2u& _tileset_pos, Random::UUID _tileset_uuid)
	: m_LayerIndex(_layer_index)
	, m_WorldPosition(_world_position)
	, m_TilesetPosition(_tileset_pos)
	, m_TilesetUUID(_tileset_uuid)
{
	sf::Vertex top_left, top_right, bottom_left, bottom_right;

	const sf::Vector2f world_position_f = { static_cast<float>(_world_position.x), static_cast<float>(_world_position.y) };
	//const float tile_size_f = static_cast<float>(Rendering::System::s_LevelTileSize);
	//const sf::Vector2f tileset_pos_f = { static_cast<float>(_tileset_pos.x) * tile_size_f, static_cast<float>(_tileset_pos.y) * tile_size_f };

	if (m_TilesetUUID == Random::EMPTY_UUID)
	{
		m_Sprite = new sf::Sprite(ApplicationSingleton::Instance().GetEngineController().GetAssetsStorage()->GetEmptyTexture());
		m_Sprite->setPosition(world_position_f);
		m_AnyTransparentPixel = false;
	}
	else
	{
		m_Sprite = nullptr;
	}
}
