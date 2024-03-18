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

			const sf::IntRect& GetArea() const { return m_Area; }

			std::shared_ptr<Serializable::Binary::ChunkInfo>& GetChunkInfo() { return m_ChunkInfo; }

	private:

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

			Level();
			~Level() = default;

			void Update(float _dt);

			ChunksManager* GetChunksManager() { return m_ChunksManager.get(); }

			void SetLevelBounds(const sf::FloatRect& _level_bounds) { m_LevelBounds = _level_bounds; }
			const sf::FloatRect& GetLevelBounds() const { return m_LevelBounds; }

			void SetTilesSize(unsigned int _tile_size);
			unsigned int GetTilesSize() const;


		private:

			sf::FloatRect m_LevelBounds;
			std::string m_LevelName;
			std::unique_ptr<ChunksManager> m_ChunksManager;

			unsigned int m_TilesSize;

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

	private:

		void ActivateLevel(const std::string& _level_name);
		void DeactivateLevel(const std::string& _level_name);

		bool IsLevelCached(const std::string& _level_name) const;
		void LoadLevel(const std::string& _level_name, const std::string& _chunk_root_name, unsigned int _tile_size);

	private:

		std::shared_ptr<Camera> m_Camera;
		std::unordered_map<std::string, std::unique_ptr<Level>> m_CachedLevels;
		std::string m_ActiveLevel;

		std::vector<Serializable::JSON::LevelInfo> m_InitialLevelsData;
	};
}