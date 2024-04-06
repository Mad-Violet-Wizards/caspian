#pragma once
#include "engine/Filesystem/FilesystemMark.hpp"
#include "engine/Core/Serializable/LevelSerializable.hpp"

class Camera;

namespace Level
{
	////////////////////////////////////////////////
	class Chunk
	{
		public:

			Chunk(const sf::IntRect& _area);
			~Chunk() = default;

			void PaintTile(const sf::Vector2u& position, const sf::Vector2u& tileset_tile_pos, unsigned int _tiles_size, unsigned int _layer, ETag _drawable_type, Random::UUID _tilset_uuid);
			void EraseTile(const sf::Vector2u& _position, unsigned int _tiles_size, unsigned int _layer, ETag _drawable_type);

			const sf::IntRect& GetArea() const { return m_Area; }

			std::shared_ptr<Serializable::Binary::ChunkInfo>& GetChunkInfo() { return m_ChunkInfo; }

	private:

			Serializable::Binary::TextureTileInfo* FindTileInfo(const sf::Vector2u& _position, unsigned int _tiles_size, unsigned int _layer, ETag _drawable_type);

			void PerformSave();


			sf::IntRect m_Area;
			std::shared_ptr<Serializable::Binary::ChunkInfo> m_ChunkInfo;
	};

	////////////////////////////////////////////////
	class ChunksManager
	{
	public:

		ChunksManager();
		~ChunksManager() = default;

		bool GenerateNewChunk(unsigned int _x, unsigned int _y, unsigned int _width_pixels, unsigned int _height_pixels, unsigned int _tile_size, const std::string& _lvl_name);
		void PushChunk(std::unique_ptr<Chunk>&& _chunk);
		const std::vector<std::unique_ptr<Chunk>>& GetChunks() const { return m_Chunks; }
		bool IsChunkRootInfoEmpty() const;

		std::shared_ptr<Serializable::Binary::ChunkRootInfo>& GetChunkRootInfo() { return m_ChunkRootInfo; }

		void Clear();

	private:

		std::shared_ptr<Serializable::Binary::ChunkRootInfo> m_ChunkRootInfo;
		std::vector<std::unique_ptr<Chunk>> m_Chunks;
	};
	////////////////////////////////////////////////
	class Level
	{
		public:

			Level(const std::string& _level_name);
			~Level() = default;

			void OnLevelActivated();
			void OnLevelDeactivated();

			void Update(float _dt);

			ChunksManager* GetChunksManager() { return m_ChunksManager.get(); }

			void SetLevelBounds(const sf::FloatRect& _level_bounds) { m_LevelBounds = _level_bounds; }
			const sf::FloatRect& GetLevelBounds() const { return m_LevelBounds; }

			void SetTilesSize(unsigned int _tile_size);
			unsigned int GetTilesSize() const;


			void SetNoBackroundLayers(unsigned int _val);
			unsigned int GetNoBackroundLayers() const;

			void SetNoForegroundLayers(unsigned int _val);
			unsigned int GetNoForegroundLayers() const;

			const std::string& GetLevelName() const;

		private:

			sf::FloatRect m_LevelBounds;
			std::string m_LevelName;
			std::unique_ptr<ChunksManager> m_ChunksManager;

			unsigned int m_TilesSize;
			unsigned int m_BackgroundNoLayersSize;
			unsigned int m_ForegroundNoLayers;

	};


	///////////////////////////////////////////////
	class World
	{
	public:

		World();
		~World() = default;


		void Update(float _dt);

		void PushInitialLevelData(std::shared_ptr<Serializable::JSON::LevelInfo>& _level_info);
		const std::vector<Serializable::JSON::LevelInfo>& GetInitialLevelsData() const { return m_InitialLevelsData; }

		bool CreateNewLevel(const std::string& _lvl_path, const std::string& _level_name, unsigned int _tile_size);
		void SwitchToLevel(const std::string& _level_name);

		std::shared_ptr<Camera> GetCamera() { return m_Camera; }

		Level* GetActiveLevel() const;
		bool IsLevelActive() const;

		void PaintTile(const sf::Vector2u& position, const sf::Vector2u& tileset_tile_pos, unsigned int _tiles_size, unsigned int _layer, ETag _drawable_type, Random::UUID _tilset_uuid);
		void EraseTile(const sf::Vector2u& _position, unsigned int _tiles_size, unsigned int _layer, ETag _drawable_type);

	private:

		void ActivateLevel(const std::string& _level_name);
		void DeactivateCurrentLevel();

		bool IsLevelCached(const std::string& _level_name) const;
		void LoadLevel(const Serializable::JSON::LevelInfo& _entry_level_info);

	private:

		std::shared_ptr<Camera> m_Camera;

		Level* m_ActiveLevelPtr;

		std::unordered_map<std::string, std::unique_ptr<Level>> m_CachedLevels;

		std::vector<Serializable::JSON::LevelInfo> m_InitialLevelsData;
	};
}