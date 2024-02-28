#pragma once

#include "engine/Core/Level.hpp"

constexpr int SPATIAL_BUCKET_SIZE = 256;

namespace Rendering
{

	////////////////////////////////////////////////////////////
	using TileIndex = std::pair<int, int>;

	struct TileIndexHash
	{
		template<class A, class B>
		std::size_t operator()(const std::pair<A, B>& _pair) const
		{
			return std::hash<A>()(_pair.first) ^ std::hash<B>()(_pair.second);
		}
	};

	// TODO: VERTEX BUFFER RENDERING!!!
	class RenderTile
	{
		public:

			RenderTile(int _layer_index, const sf::Vector2u& _world_position, const sf::Vector2u& _tileset_pos, Random::UUID _tileset_uuid);
			~RenderTile() = default;

			int GetLayerIndex() const { return m_LayerIndex; }
			const sf::Vector2u& GetWorldPosition() const { return m_WorldPosition; }
			const sf::Vector2u& GetTilesetPosition() const { return m_TilesetPosition; }
			Random::UUID GetTilesetUUID() const { return m_TilesetUUID; }

			const std::array<sf::Vertex, 4>& GetVertices() const;

		private:

			int m_LayerIndex;

			sf::Vector2u m_WorldPosition;
			sf::Vector2u m_TilesetPosition;
			Random::UUID m_TilesetUUID;

			std::array<sf::Vertex, 4> m_Vertices;
	};

	class SpatialHashGrid
	{
		public:

			SpatialHashGrid() = default;
			~SpatialHashGrid() = default;

			void ProcessRenderTile(const RenderTile& _render_tile);

			bool Contains(const TileIndex& _index) const { return m_SpatialHash.find(_index) != m_SpatialHash.end(); }
			const std::vector<RenderTile>& GetTiles(const TileIndex& _index) const { return m_SpatialHash.at(_index); }

			TileIndex GetTileIndex(const sf::Vector2u& _pos) const;
			TileIndex GetTileIndexVecFloat(const sf::Vector2f& _pos) const;

			size_t GetSize() const { return m_SpatialHash.size(); }

			void Clear();

		private:

			std::unordered_map<TileIndex, std::vector<RenderTile>, TileIndexHash> m_SpatialHash;

	};

	////////////////////////////////////////////////////////////
	class LevelRendering
	{
		public:

			LevelRendering() = default;
			~LevelRendering() = default;

			void Render(sf::RenderWindow& _window);

			void MarkChunk(Random::UUID _chunk_uuid);
			bool ContainsChunk(Random::UUID _chunk_uuid) const;
			bool IsEmpty() const { return m_CurrentChunks.empty(); }

			SpatialHashGrid& GetSpatialHashGrid() { return m_SpatialHashGrid; }

			void Clear();

	private:

			// Method return ALL visible tiles, including even those overlapped by other tiles on different layers.
			std::vector<RenderTile> GetVisibleTiles() const;

			std::map<std::pair<int, Random::UUID>, std::vector<RenderTile>> GroupTilesByLayer(const std::vector<RenderTile>& _visible_tiles) const;

		private:

			SpatialHashGrid m_SpatialHashGrid;
			std::vector<Random::UUID> m_CurrentChunks;
	};


	////////////////////////////////////////////////////////////
	class System
	{
		public:

			System() = default;
			~System() = default;

			void Render(sf::RenderWindow& _window);
			
			void OnLevelActivated(Level::Level* _level);
			void OnLevelDeactivated(Level::Level* _level);

			void ProcessLevelChunk(Level::Chunk* _level_chunk);

			static unsigned int s_LevelTileSize;
			static sf::Vector2f s_RenderSize;

		private:
			
			LevelRendering m_LevelRendering;
	};
}