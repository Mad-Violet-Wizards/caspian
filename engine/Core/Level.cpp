#include "engine/pch.hpp"

#include <vendor/include/nlohmann/json.hpp>
#include "Level.hpp"

namespace Level
{
	World::World()
	{
		m_Camera = std::make_unique<Camera>();
	}

	//////////////////////////////////////////////////////////////////////////
	// LEVEL
	Level::Level()
	{
		m_ChunksManager = std::make_unique<ChunksManager>();
	}

	//////////////////////////////////////////////////////////////////////////
	// WORLD
	void World::ActivateLevel(const std::string& _level_name)
	{

	}

	void World::DeactivateLevel(const std::string& _level_name)
	{

	}

	bool World::IsLevelCached(const std::string& _level_name) const
	{
		return m_CachedLevels.find(_level_name) != m_CachedLevels.end();
	}

	void World::LoadLevel(const std::string& _level_name, const std::string& _chunk_root_name)
	{
		std::unique_ptr<Level> loaded_level = std::make_unique<Level>();

		fs::IFileSystem* data_fs = ApplicationSingleton::Instance().GetEngineModule().GetFilesystemManager()->Get("data");
		std::filesystem::path chunk_root_path = "levels";
		chunk_root_path /= _chunk_root_name;

		// Load chunk root file.
		if (std::shared_ptr<fs::IFile> chunk_root_file = data_fs->OpenFile(chunk_root_path.string(), fs::io::OpenMode::ReadWrite))
		{
			auto& chunk_root_info = loaded_level->GetChunksManager()->GetChunkRootInfo();
			auto binary_data = std::dynamic_pointer_cast<ISerializable::Binary>(chunk_root_info);
			chunk_root_file->DeserializeBinary(binary_data);
			chunk_root_info = std::dynamic_pointer_cast<Serializable::Binary::ChunkRootInfo>(binary_data);
		}
	}

	void World::SwitchToLevel(const std::string& _level_name)
	{
		if (_level_name == m_ActiveLevel)
		{
			std::cout << "[WORLD] Cannot switch to level: " << _level_name << " because it's already active.";
			return;
		}

		// Check JSON for chunk root file.
		for (const auto& level_info : m_InitialLevelsData)
		{
			if (IsLevelCached(_level_name))
			{
				ActivateLevel(_level_name);
			}
			else
			{
				LoadLevel(_level_name, level_info.m_ChunkRootFile);
				ActivateLevel(_level_name);
			}
		}
	}

	void World::Draw()
	{

	}

	void World::Update(float _dt)
	{

	}

	// TODO: Waste of memory & dandlding std::shared_ptr...
	void World::PushInitialLevelData(std::shared_ptr<Serializable::JSON::LevelInfo>& _level_info)
	{
		m_InitialLevelsData.push_back({ _level_info->m_LevelName, _level_info->m_ChunkRootFile, _level_info->m_TileWidth, _level_info->m_TileHeight });
	}

	bool World::CreateNewLevel(const std::string& _lvl_path, const std::string& _level_name, unsigned int _tile_width, unsigned int _tile_height)
	{
		std::unique_ptr<Level> new_level = std::make_unique<Level>();

		auto& main_instance = ApplicationSingleton::Instance();
		fs::IFileSystem* resource_fs = main_instance.GetEngineModule().GetFilesystemManager()->Get("resources");

		bool json_created_succesfully = false;

		std::string relative_lvl_path{ _lvl_path };
		relative_lvl_path = relative_lvl_path.substr(resource_fs->GetPath().size() + 1);

		if (!resource_fs->FileExists(relative_lvl_path))
		{
			json_created_succesfully = resource_fs->CreateFile(relative_lvl_path, fs::IFile::EType::JSON);

			if (!json_created_succesfully)
			{
				std::cout << "DEBUG: [ToolsImpl] Failed to create level file: " << relative_lvl_path << std::endl;
			}
		}

		bool file_closed_successfully = false;
		bool json_serialized = false;

		if (std::shared_ptr<fs::IFile> level_json_file = resource_fs->OpenFile(relative_lvl_path, fs::io::OpenMode::ReadWrite))
		{
			const std::string& root_chunk_file_name = _level_name + ".rootchunk";
			std::shared_ptr<ISerializable::JSON> level_data = std::make_shared<Serializable::JSON::LevelInfo>(_level_name, root_chunk_file_name, _tile_width, _tile_height);
			level_json_file->SerializeJson(level_data);

			std::shared_ptr<Serializable::JSON::LevelInfo> level_info = std::dynamic_pointer_cast<Serializable::JSON::LevelInfo>(level_data);
			main_instance.GetWorld()->PushInitialLevelData(level_info);

			json_serialized = level_json_file->Size() > 0;
			file_closed_successfully = resource_fs->CloseFile(level_json_file);
		}

		fs::IFileSystem* data_fs = main_instance.GetEngineModule().GetFilesystemManager()->Get("data");

		if (!data_fs->FileExists("levels"))
		{
			data_fs->CreateFile("levels", fs::IFile::EType::Directory);
		}

		ChunksManager* chunks_manager = new_level->GetChunksManager();

		bool chunk_processed = false;

		if (chunks_manager->IsChunkRootInfoEmpty())
		{
			// Create chunk from scratch as map is empty.
			chunk_processed = chunks_manager->GenerateNewChunk(0, 0, 8192, 8192, 32, _level_name);
		}

		const bool success = json_created_succesfully && file_closed_successfully && json_serialized && chunk_processed;

		if (success)
		{
			m_CachedLevels.insert({ _level_name, std::move(new_level) });
			ActivateLevel(_level_name);
			return true;
		}
		else
		{
			return false;
		}
	
		return json_created_succesfully && file_closed_successfully && json_serialized && chunk_processed;
	}

	//////////////////////////////////////////////////////////////////
	// CHUNK
	Chunk::Chunk()
	{
		m_ChunkInfo = std::make_shared<Serializable::Binary::ChunkInfo>();
	}

	//////////////////////////////////////////////////////////////////
	// CHUNK MANAGER
	ChunksManager::ChunksManager()
	{
		m_ChunkRootInfo = std::make_shared<Serializable::Binary::ChunkRootInfo>();
	}

	bool ChunksManager::GenerateNewChunk(unsigned int _x, unsigned int _y, unsigned int _width_pixels, unsigned int _height_pixels, unsigned int _tile_size, const std::string& _lvl_name)
	{
		Random::UUID new_chunk_uuid;

		// For now let's consider the tile size is same - so width=height.

		const std::string chunk_file_name = Random::GenerateRandomString(16) + fs::IFile::TypeToStringExt(fs::IFile::EType::Data_LevelChunk);

		fs::IFileSystem* data_fs = ApplicationSingleton::Instance().GetEngineModule().GetFilesystemManager()->Get("data");

		std::filesystem::path chunk_file_path = data_fs->GetPath();
		chunk_file_path /= "levels";
		chunk_file_path /= chunk_file_name;

		if (!data_fs->FileExists(chunk_file_path.string()))
		{
			data_fs->CreateFile(chunk_file_path.string(), fs::IFile::EType::Data_LevelChunk);
		}

		std::filesystem::path chunk_root_file_path = data_fs->GetPath();
		chunk_root_file_path /= "levels";
		chunk_root_file_path /= _lvl_name + fs::IFile::TypeToStringExt(fs::IFile::EType::Data_LevelRootChunk);

		if (!data_fs->FileExists(chunk_root_file_path.string()))
		{
			data_fs->CreateFile(chunk_root_file_path.string(), fs::IFile::EType::Data_LevelRootChunk);
		}

		bool chunk_file_closed = false;
		bool chunk_file_serialized = false;

		std::unique_ptr<Chunk> new_chunk = std::make_unique<Chunk>();

		if (std::shared_ptr<fs::IFile> chunk_file = data_fs->OpenFile(chunk_file_path.string(), fs::io::OpenMode::ReadWrite))
		{
			auto chunk_info = new_chunk->GetChunkInfo();
			chunk_info->m_ChunkUUID = new_chunk_uuid;

			Serializable::Binary::TextureLayerInfo default_layer;
			default_layer.m_LayerIndex = 0;
			default_layer.m_Tiles.resize((_width_pixels / _tile_size) * (_height_pixels / _tile_size));
			chunk_info->m_TileLayers.push_back(default_layer);

			chunk_file->SerializeBinary(new_chunk->GetChunkInfo());

			chunk_file_serialized = chunk_file->Size() > 0;
			chunk_file_closed = data_fs->CloseFile(chunk_file);
		}

		Serializable::Binary::ChunkRoot_ChunkInfo chunkroot_chunk_info;
		chunkroot_chunk_info.m_RelativePath = chunk_file_name;
		chunkroot_chunk_info.m_ChunkUUID = new_chunk_uuid;
		chunkroot_chunk_info.m_ChunkPosX.x = _x;
		chunkroot_chunk_info.m_ChunkPosX.y = _width_pixels;
		chunkroot_chunk_info.m_ChunkPosY.x = _y;
		chunkroot_chunk_info.m_ChunkPosY.y = _height_pixels;

		bool chunk_root_file_closed = false;
		bool chunk_root_file_serialized = false;

		if (chunk_file_serialized && chunk_file_closed)
		{
			if (std::shared_ptr<fs::IFile> chunk_root_file = data_fs->OpenFile(chunk_root_file_path.string(), fs::io::OpenMode::ReadWrite))
			{
				chunk_root_file->SerializeBinary(m_ChunkRootInfo);
				chunk_root_file_serialized = chunk_root_file->Size() > 0;
				chunk_root_file_closed = data_fs->CloseFile(chunk_root_file);
			}
		}

		if (chunk_root_file_serialized && chunk_root_file_closed && chunk_file_closed && chunk_file_serialized)
		{
			m_ChunkRootInfo->m_ChunksInfoVec.push_back(chunkroot_chunk_info);
			m_Chunks.push_back(std::move(new_chunk));

			return true;
		}
		else [[unlikely]]
			{
				new_chunk.reset();

				return false;
			}

	}

	bool ChunksManager::IsChunkRootInfoEmpty() const
	{
		return m_ChunkRootInfo->m_ChunksInfoVec.empty();
	}

	void ChunksManager::Clear()
	{
		m_Chunks.clear();
		m_ChunkRootInfo->m_ChunksInfoVec.clear();
	}
} // namespace Level