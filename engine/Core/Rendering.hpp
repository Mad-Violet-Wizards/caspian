#pragma once

#include "engine/Core/Level.hpp"

constexpr int SPATIAL_BUCKET_SIZE = 256;

namespace Rendering
{

	///////////////////////////////////////////////////////
	class TextureLayer
	{
		public:

			TextureLayer(unsigned int _size = 0);
			~TextureLayer() = default;
			
			void SetIndex(int _index) { m_LayerIndex = _index; }
			int GetIndex() const { return m_LayerIndex; }

			void AddSprite(sf::Sprite* _sprite);

			const std::vector<sf::Sprite*>& GetSprites() const { return m_CachedSprites; }

		private:

			int m_LayerIndex;
			std::vector<sf::Sprite*> m_CachedSprites;

	};

	////////////////////////////////////////////////////////////
	class RenderChunk
	{
		public:

			RenderChunk(Random::UUID _owning_chunk_uuid = Random::EMPTY_UUID);
			~RenderChunk() = default;

			void AddTextureLayer(TextureLayer& _texture_layer) { m_TextureLayers.push_back(_texture_layer); }
			Random::UUID GetUUID() const { return m_OwningChunkUUID; }

		private:

			Random::UUID m_OwningChunkUUID;

			std::vector<TextureLayer> m_TextureLayers;
	};

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

			RenderTile(const sf::Vector2u& _world_position, const sf::IntRect& _texture_rect, Random::UUID _tileset_uuid);
			~RenderTile() = default;

			sf::Vector2u GetWorldPosition() const { return m_WorldPosition; }
			sf::IntRect GetTextureRect() const { return m_TextureRect; }
			Random::UUID GetTilesetUUID() const { return m_TilesetUUID; }

		private:

			sf::Vector2u m_WorldPosition;
			sf::IntRect m_TextureRect;
			Random::UUID m_TilesetUUID;
	};

	class SpatialHashGrid
	{
		public:

			SpatialHashGrid() = default;
			~SpatialHashGrid() = default;

			void ProcessTextureTileLayer(const TextureLayer& _texture_layer);

			bool Contains(const TileIndex& _index) const { return m_SpatialHash.find(_index) != m_SpatialHash.end(); }
			const std::vector<sf::Sprite*>& FindSpatialBucket(const TileIndex& _index) const;

			TileIndex GetTileIndex(const sf::Vector2f& _pos) const;

			size_t GetSize() const { return m_SpatialHash.size(); }

		private:

			std::unordered_map<TileIndex, std::vector<sf::Sprite*>, TileIndexHash> m_SpatialHash;

	};

	////////////////////////////////////////////////////////////
	class LevelRendering
	{
		public:

			LevelRendering() = default;
			~LevelRendering() = default;

			void Render(sf::RenderWindow& _window);
			bool ContainsChunk(Random::UUID _chunk_uuid) const;
			void AddChunk(RenderChunk _chunk) { m_RenderChunks.push_back(_chunk); }
			bool IsEmpty() const { return m_RenderChunks.empty(); }

			void AddToSpatialHash(const TextureLayer& _texture_layer);

		private:

			SpatialHashGrid m_SpatialHashGrid;
			std::vector<RenderChunk> m_RenderChunks;
	};


	////////////////////////////////////////////////////////////
	class System
	{
		public:

			System() = default;
			~System() = default;

			void Render(sf::RenderWindow& _window);
			
			void OnLevelActivated(Level::Level* _level);

			void ProcessChunk(Level::Chunk* _level_chunk);

		private:
			
			LevelRendering m_LevelRendering;

			static unsigned int s_TileSize;
	};
}