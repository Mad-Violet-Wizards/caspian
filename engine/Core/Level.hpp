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

			Chunk();
			~Chunk() = default;

			std::shared_ptr<Serializable::Binary::ChunkInfo>& GetChunkInfo() { return m_ChunkInfo; }

	private:

			std::shared_ptr<Serializable::Binary::ChunkInfo> m_ChunkInfo;
	};

	////////////////////////////////////////////////
	class ChunksManager
	{
	public:

		ChunksManager();
		~ChunksManager() = default;

		bool GenerateNewChunk(unsigned int _x, unsigned int _y, unsigned int _width_pixels, unsigned int _height_pixels, unsigned int _tile_size, const std::string& _lvl_name);
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

			ChunksManager* GetChunksManager() { return m_ChunksManager.get(); }


		private:

			std::unique_ptr<ChunksManager> m_ChunksManager;

	};


	///////////////////////////////////////////////
	class World
	{
	public:

		World();
		~World() = default;

		void SwitchToLevel(const std::string& _level_name);

		void Draw();
		void Update(float _dt);

		void PushInitialLevelData(std::shared_ptr<Serializable::JSON::LevelInfo>& _level_info);
		const std::vector<Serializable::JSON::LevelInfo>& GetInitialLevelsData() const { return m_InitialLevelsData; }

		bool CreateNewLevel(const std::string& _lvl_path, const std::string& _level_name, unsigned int _tile_width, unsigned int _tile_height);

		Camera* GetCamera() { return m_Camera.get(); }

	private:

		void ActivateLevel(const std::string& _level_name);
		void DeactivateLevel(const std::string& _level_name);

		bool IsLevelCached(const std::string& _level_name) const;
		void LoadLevel(const std::string& _level_name, const std::string& _chunk_root_name);

	private:

		std::unique_ptr<Camera> m_Camera;
		std::unordered_map<std::string, std::unique_ptr<Level>> m_CachedLevels;
		std::string m_ActiveLevel;

		std::vector<Serializable::JSON::LevelInfo> m_InitialLevelsData;
	};
}