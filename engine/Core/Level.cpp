#include "engine/pch.hpp"

#include <vendor/include/nlohmann/json.hpp>
#include "Level.hpp"

namespace Levels
{
	World::World()
	{
		m_Camera = std::make_shared<Camera>();
		m_ActiveLevelPtr = nullptr;
	}

	//////////////////////////////////////////////////////////////////////////
	// LEVEL
	Level::Level(const std::string& _level_name)
		: m_LevelName(_level_name)
		, m_TilesSize(0)
		, m_BackgroundNoLayersSize(0)
		, m_ForegroundNoLayers(0)
	{
		m_ChunksManager = std::make_unique<ChunksManager>();
	}

	void Level::OnLevelActivated()
	{

	}

	void Level::OnLevelDeactivated()
	{

	}

	void Level::Update(float _dt)
	{
	}

	void Level::SetTilesSize(unsigned int _tile_size)
	{
		m_TilesSize = _tile_size;
	}

	unsigned int Level::GetTilesSize() const
	{
		return m_TilesSize;
	}

	void Level::SetNoBackroundLayers(unsigned int _val)
	{
		m_BackgroundNoLayersSize = _val;
	}

	unsigned int Level::GetNoBackroundLayers() const
	{
		return m_BackgroundNoLayersSize;
	}

	void Level::SetNoForegroundLayers(unsigned int _val)
	{
		m_ForegroundNoLayers = _val;
	}

	unsigned int Level::GetNoForegroundLayers() const
	{
		return m_ForegroundNoLayers;
	}

	const std::string& Level::GetLevelName() const
	{
		return m_LevelName;
	}

	//////////////////////////////////////////////////////////////////////////
	// WORLD
	void World::ActivateLevel(const std::string& _level_name)
	{
		m_ActiveLevelPtr = m_CachedLevels[_level_name].get();

		m_Camera->SetLevelBounds(m_ActiveLevelPtr->GetLevelBounds());

		GameObjectCollection* game_objects_collection = ApplicationSingleton::Instance().GetEngineController().GetGameObjectStorage();
		game_objects_collection->SortDrawablesOnDirtyFlag(false);
		// Or in reverse, not sure.
		for (const auto& chunk : m_ActiveLevelPtr->GetChunksManager()->GetChunks())
		{
			const auto& chunk_info = chunk->GetChunkInfo();
			for (const Serializable::Binary::TextureLayerInfo& background_tile_data : chunk_info->m_BackgroundTileLayers)
			{
				for (const auto& tile_info : background_tile_data.m_Tiles)
					if (tile_info.m_TilesetUUID != Random::EMPTY_UUID)
						game_objects_collection->ConstructNew(tile_info, ETag::Drawable_Background, m_ActiveLevelPtr->GetTilesSize(), background_tile_data.m_LayerIndex);


				for (const Serializable::Binary::TextureLayerInfo& foreground_tile_data : chunk_info->m_ForegroundTileLayers)
				{
					for (const auto& tile_info : foreground_tile_data.m_Tiles)
						if (tile_info.m_TilesetUUID != Random::EMPTY_UUID)
							game_objects_collection->ConstructNew(tile_info, ETag::Drawable_Foreground, m_ActiveLevelPtr->GetTilesSize(), foreground_tile_data.m_LayerIndex);
				}
			}
		}

		game_objects_collection->SortDrawablesOnDirtyFlag(true);

		ApplicationSingleton::Instance().GetEngineController().GetCollisionsManager()->OnLevelActivated(m_ActiveLevelPtr);
	}

	void World::DeactivateCurrentLevel()
	{
	}

	bool World::IsLevelCached(const std::string& _level_name) const
	{
		return m_CachedLevels.find(_level_name) != m_CachedLevels.end();
	}

	void World::LoadLevel(const Serializable::JSON::LevelInfo& _entry_level_info)
	{
		std::unique_ptr<Level> loaded_level = std::make_unique<Level>(_entry_level_info.m_LevelName);
		sf::FloatRect level_bounds(0.f, 0.f, 0.f, 0.f);

		fs::IFileSystem* data_fs = ApplicationSingleton::Instance().GetEngineController().GetFilesystemManager()->Get("data");
		std::filesystem::path collisions_path = "levels";
		collisions_path /= _entry_level_info.m_CollisionsInfoFile;

		// Load collisions file.
		if (std::shared_ptr<fs::IFile> collision_file = data_fs->OpenFile(collisions_path.string(), fs::io::OpenMode::In | fs::io::OpenMode::Binary))
		{
			Collisions::Manager* collisions_manager = ApplicationSingleton::Instance().GetEngineController().GetCollisionsManager();
			
			std::shared_ptr<Serializable::Binary::CollisionLayerInfo> collision_layer_info = std::make_shared<Serializable::Binary::CollisionLayerInfo>();

			if (collision_file->Size() > 0)
			{
				auto binary_data = std::dynamic_pointer_cast<ISerializable::Binary>(collision_layer_info);
				collision_file->Seek(0, fs::io::Origin::Begin);
				collision_file->DeserializeBinary(binary_data);
				collision_layer_info = std::dynamic_pointer_cast<Serializable::Binary::CollisionLayerInfo>(binary_data);
			}

			collisions_manager->AddCachedInfo(_entry_level_info.m_LevelName, collision_layer_info);


			collision_file->Close();
		}

		std::filesystem::path chunk_root_path = "levels";
		chunk_root_path /= _entry_level_info.m_ChunkRootFile;

		// Load chunk root file.
		if (std::shared_ptr<fs::IFile> chunk_root_file = data_fs->OpenFile(chunk_root_path.string(), fs::io::OpenMode::In | fs::io::OpenMode::Binary))
		{
			auto& chunk_root_info = loaded_level->GetChunksManager()->GetChunkRootInfo();
			auto binary_data = std::dynamic_pointer_cast<ISerializable::Binary>(chunk_root_info);
			chunk_root_file->Seek(0, fs::io::Origin::Begin);
			chunk_root_file->DeserializeBinary(binary_data);
			chunk_root_info = std::dynamic_pointer_cast<Serializable::Binary::ChunkRootInfo>(binary_data);

#define CAST_INT(x) static_cast<int>(x)

			for (const auto& chunk_info : chunk_root_info->m_ChunksInfoVec)
			{
				const sf::IntRect chunk_rect = { CAST_INT(chunk_info.m_ChunkPosX.x),
																				 CAST_INT(chunk_info.m_ChunkPosY.x),
																				 CAST_INT(chunk_info.m_ChunkPosX.y),
																				 CAST_INT(chunk_info.m_ChunkPosY.y) };

				std::filesystem::path chunk_path = "levels";
				chunk_path /= chunk_info.m_RelativePath;

				if (std::shared_ptr<fs::IFile> chunk_file = data_fs->OpenFile(chunk_path.string(), fs::io::OpenMode::In | fs::io::OpenMode::Binary))
				{
					std::unique_ptr<Chunk> chunk = std::make_unique<Chunk>(chunk_rect);
					auto& parsed_chunk_info = chunk->GetChunkInfo();
					auto binary_chunk_data = std::dynamic_pointer_cast<ISerializable::Binary>(parsed_chunk_info);
					chunk_file->Seek(0, fs::io::Origin::Begin);
					chunk_file->DeserializeBinary(binary_chunk_data);

					parsed_chunk_info = std::dynamic_pointer_cast<Serializable::Binary::ChunkInfo>(binary_chunk_data);
					parsed_chunk_info->m_ChunkUUID = chunk_info.m_ChunkUUID;

					loaded_level->SetNoBackroundLayers(chunk->GetChunkInfo()->m_BackgroundTileLayers.size());
					loaded_level->SetNoForegroundLayers(chunk->GetChunkInfo()->m_ForegroundTileLayers.size());

					loaded_level->GetChunksManager()->PushChunk(std::move(chunk));

					level_bounds.width += chunk_rect.width;
					level_bounds.height += chunk_rect.height;

					data_fs->CloseFile(chunk_file);

					// TODO: For now we can break, in future we need to load neighbouring chunks.
				}

				data_fs->CloseFile(chunk_root_file);
			}
		}

		loaded_level->SetLevelBounds(level_bounds);
		loaded_level->SetTilesSize(_entry_level_info.m_TileSize);
		m_CachedLevels.insert({ _entry_level_info.m_LevelName, std::move(loaded_level) });
	}

	void World::SwitchToLevel(const std::string& _level_name)
	{
		if (m_ActiveLevelPtr)
		{
			if (_level_name == m_ActiveLevelPtr->GetLevelName())
			{
				std::cout << "[WORLD] Cannot switch to level: " << _level_name << " because it's already active.";
				return;
			}
		}

		// Check JSON for chunk root file.
		for (const auto& level_info : m_InitialLevelsData)
		{
			if (level_info.m_LevelName == _level_name)
			{
				if (m_ActiveLevelPtr)
					DeactivateCurrentLevel();

				if (IsLevelCached(_level_name))
				{
					ActivateLevel(_level_name);
				}
				else
				{
					LoadLevel(level_info);
					ActivateLevel(_level_name);
				}
			}
		}
	}

	Level* World::GetActiveLevel() const
	{
		return m_ActiveLevelPtr;
	}

	bool World::IsLevelActive() const
	{
		return m_ActiveLevelPtr != nullptr;
	}

	void World::PaintTile(const sf::Vector2u& position, const sf::Vector2u& tileset_tile_pos, unsigned int _tiles_size, unsigned int _layer, ETag _drawable_type, Random::UUID _tilset_uuid)
	{
		if (Level* current_level = GetActiveLevel())
		{
			const std::vector<std::unique_ptr<Chunk>>& chunks = current_level->GetChunksManager()->GetChunks();

			for (const auto& chunk : chunks)
			{
				if (chunk->GetArea().contains(sf::Vector2i(position)))
				{
					chunk->PaintTile(position, tileset_tile_pos, _tiles_size, _layer, _drawable_type, _tilset_uuid);
				}
			}
		}
	}

	void World::EraseTile(const sf::Vector2u& _position, unsigned int _tiles_size, unsigned int _layer, ETag _drawable_type)
	{
		if (Level* current_level = GetActiveLevel())
		{
			const std::vector<std::unique_ptr<Chunk>>& chunks = current_level->GetChunksManager()->GetChunks();

			for (const auto& chunk : chunks)
			{
				if (chunk->GetArea().contains(sf::Vector2i(_position)))
				{
					chunk->EraseTile(_position, _tiles_size, _layer, _drawable_type);
				}
			}
		}
	}

	void World::Update(float _dt)
	{
		m_Camera->Update(_dt);
		
		if (m_ActiveLevelPtr)
			m_ActiveLevelPtr->Update(_dt);
	}

	// TODO: Waste of memory & dandlding std::shared_ptr...
	void World::PushInitialLevelData(std::shared_ptr<Serializable::JSON::LevelInfo>& _level_info)
	{
		m_InitialLevelsData.push_back({ _level_info->m_LevelName, _level_info->m_ChunkRootFile, _level_info->m_CollisionsInfoFile, _level_info->m_TileSize });
	}

	bool World::CreateNewLevel(const std::string& _lvl_path, const std::string& _level_name, unsigned int _tile_size)
	{
		std::unique_ptr<Level> new_level = std::make_unique<Level>(_level_name);

		auto& main_instance = ApplicationSingleton::Instance();
		fs::IFileSystem* resource_fs = main_instance.GetEngineController().GetFilesystemManager()->Get("resources");

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

		const std::string& collisions_file_name = _level_name + fs::IFile::TypeToStringExt(fs::IFile::EType::Data_Collisions);

		if (std::shared_ptr<fs::IFile> level_json_file = resource_fs->OpenFile(relative_lvl_path, fs::io::OpenMode::Out))
		{
			const std::string& root_chunk_file_name = _level_name + fs::IFile::TypeToStringExt(fs::IFile::EType::Data_LevelRootChunk);
			std::shared_ptr<ISerializable::JSON> level_data = std::make_shared<Serializable::JSON::LevelInfo>(_level_name, root_chunk_file_name, collisions_file_name, _tile_size);
			level_json_file->SerializeJson(level_data);

			std::shared_ptr<Serializable::JSON::LevelInfo> level_info = std::dynamic_pointer_cast<Serializable::JSON::LevelInfo>(level_data);
			main_instance.GetWorld()->PushInitialLevelData(level_info);

			json_serialized = level_json_file->Size() > 0;
			file_closed_successfully = resource_fs->CloseFile(level_json_file);
		}

		fs::IFileSystem* data_fs = main_instance.GetEngineController().GetFilesystemManager()->Get("data");

		if (!data_fs->FileExists("levels"))
		{
			data_fs->CreateFile("levels", fs::IFile::EType::Directory);
		}

		// Create chunk from scratch as map is empty.
		ChunksManager* chunks_manager = new_level->GetChunksManager();
		bool chunk_processed = false;
		if (chunks_manager->IsChunkRootInfoEmpty())
		{
			chunk_processed = chunks_manager->GenerateNewChunk(0, 0, 4096, 4096, _tile_size, _level_name);
		}

		std::filesystem::path collisions_file_path = "levels";
		collisions_file_path /= collisions_file_name;

		if (!data_fs->FileExists(collisions_file_path.string()))
		{
			data_fs->CreateFile(collisions_file_path.string(), fs::IFile::EType::Data_Collisions);
		}

		const bool success = json_created_succesfully && file_closed_successfully && json_serialized && chunk_processed;

		if (success)
		{
			new_level->SetLevelBounds({ 0.f, 0.f, 8192.f, 8192.f });
			new_level->SetTilesSize(_tile_size);
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
	Chunk::Chunk(const sf::IntRect& _area)
		: m_Area(_area)
	{
		m_ChunkInfo = std::make_shared<Serializable::Binary::ChunkInfo>();
	}

	void Chunk::PaintTile(const sf::Vector2u& position, const sf::Vector2u& tileset_tile_pos, unsigned int _tiles_size, unsigned int _layer, ETag _drawable_type, Random::UUID _tilset_uuid)
	{
		if (Serializable::Binary::TextureTileInfo* tile_info = FindTileInfo(position, _tiles_size, _layer, _drawable_type))
		{
			tile_info->m_TilesetUUID = _tilset_uuid;
			tile_info->m_TilePositionX = position.x;
			tile_info->m_TilePositionY = position.y;
			tile_info->m_TilesetColumn = tileset_tile_pos.x;
			tile_info->m_TilesetRow = tileset_tile_pos.y;

			SpatialHashGrid* spatial_hash_grid = ApplicationSingleton::Instance().GetEngineController().GetGameObjectStorage()->GetSpatialHashGrid();

			const TileIndex tile_index = spatial_hash_grid->CalculateTileIndex(position.x, position.y);
			SpatialHashGridBucket& game_objects_bucket = spatial_hash_grid->GetRefToBucket(tile_index);
			bool tile_replaced = false;

			switch (_drawable_type)
			{
				case ETag::Drawable_Background:
				{
					for (auto it = game_objects_bucket.background_objects_begin(); it != game_objects_bucket.background_objects_end(); ++it)
					{
						if ((*it)->GetComponent<C_Sprite>()->GetLayerIndex() == _layer)
						{
							const sf::Vector2u tile_position = (sf::Vector2u)(*it)->GetComponent<C_Transform>()->GetPosition();

							if (tile_position == position)
							{
								(*it)->QueueForRemoval();
								break;
							}
						}
					}

					break;
				}

				case ETag::Drawable_Entity:
				{
					for (auto it = game_objects_bucket.entity_objects_begin(); it != game_objects_bucket.entity_objects_end(); ++it)
					{
						if ((*it)->GetComponent<C_Sprite>()->GetLayerIndex() == _layer)
						{
							const sf::Vector2u tile_position = (sf::Vector2u)(*it)->GetComponent<C_Transform>()->GetPosition();

							if (tile_position == position)
							{
								(*it)->QueueForRemoval();
								break;
							}
						}
					}

				break;
			}

				case ETag::Drawable_Foreground:
				{
					for (auto it = game_objects_bucket.foreground_objects_begin(); it != game_objects_bucket.foreground_objects_end(); ++it)
					{
						if ((*it)->GetComponent<C_Sprite>()->GetLayerIndex() == _layer)
						{
							const sf::Vector2u tile_position = (sf::Vector2u)(*it)->GetComponent<C_Transform>()->GetPosition();

							if (tile_position == position)
							{
								(*it)->QueueForRemoval();
								break;
							}
						}
					}

					break;
				}
			}

			ApplicationSingleton::Instance().GetEngineController().GetGameObjectStorage()->ConstructNew(*tile_info, _drawable_type, _tiles_size, _layer);

			PerformSave();
		}
	}

	void Chunk::EraseTile(const sf::Vector2u& _position, unsigned int _tiles_size, unsigned int _layer, ETag _drawable_type)
	{
		if (Serializable::Binary::TextureTileInfo* tile_info = FindTileInfo(_position, _tiles_size, _layer, _drawable_type))
		{
			tile_info->m_TilesetUUID = Random::EMPTY_UUID;
			tile_info->m_TilesetColumn = 0;
			tile_info->m_TilesetRow = 0;

			SpatialHashGrid* spatial_hash_grid = ApplicationSingleton::Instance().GetEngineController().GetGameObjectStorage()->GetSpatialHashGrid();

			const TileIndex tile_index = spatial_hash_grid->CalculateTileIndex(_position.x, _position.y);
			SpatialHashGridBucket& game_objects_bucket = spatial_hash_grid->GetRefToBucket(tile_index);

			switch (_drawable_type)
			{
			case ETag::Drawable_Background:
			{
				for (auto it = game_objects_bucket.background_objects_begin(); it != game_objects_bucket.background_objects_end(); ++it)
				{
					if ((*it)->GetComponent<C_Sprite>()->GetLayerIndex() == _layer)
					{
						const sf::Vector2u tile_position = (sf::Vector2u)(*it)->GetComponent<C_Transform>()->GetPosition();

						if (tile_position == _position)
						{
							(*it)->QueueForRemoval();
							break;
						}
					}
				}
				break;
			}

			case ETag::Drawable_Entity:
			{
				for (auto it = game_objects_bucket.entity_objects_begin(); it != game_objects_bucket.entity_objects_end(); ++it)
				{
					if ((*it)->GetComponent<C_Sprite>()->GetLayerIndex() == _layer)
					{
						const sf::Vector2u tile_position = (sf::Vector2u)(*it)->GetComponent<C_Transform>()->GetPosition();

						if (tile_position == _position)
						{
							(*it)->QueueForRemoval();
							break;
						}
					}
				}
				break;
			}

			case ETag::Drawable_Foreground:
			{
				for (auto it = game_objects_bucket.foreground_objects_begin(); it != game_objects_bucket.foreground_objects_end(); ++it)
				{
					if ((*it)->GetComponent<C_Sprite>()->GetLayerIndex() == _layer)
					{
						const sf::Vector2u tile_position = (sf::Vector2u)(*it)->GetComponent<C_Transform>()->GetPosition();

						if (tile_position == _position)
						{
							(*it)->QueueForRemoval();
							break;
						}
					}
				}
				break;
			}
			}

		}

		PerformSave();
	}

	Serializable::Binary::TextureTileInfo* Chunk::FindTileInfo(const sf::Vector2u& _position, unsigned int _tiles_size, unsigned int _layer, ETag _drawable_type)
	{
		if (m_ChunkInfo)
		{
			if (_drawable_type == ETag::Drawable_Background)
			{
				for (TextureLayerInfo& background_layer : m_ChunkInfo->m_BackgroundTileLayers)
				{
					for (Serializable::Binary::TextureTileInfo& tile_info : background_layer.m_Tiles)
					{
						if (tile_info.m_TilePositionX == _position.x && tile_info.m_TilePositionY == _position.y)
						{
							return &tile_info;
						}
					}
				}
			}

			if (_drawable_type == ETag::Drawable_Foreground)
			{
				for (TextureLayerInfo& foreground_layer : m_ChunkInfo->m_ForegroundTileLayers)
				{
					for (Serializable::Binary::TextureTileInfo& tile_info : foreground_layer.m_Tiles)
					{
						if (tile_info.m_TilePositionX == _position.x && tile_info.m_TilePositionY == _position.y)
						{
							return &tile_info;
						}
					}
				}
			}
		}

		return nullptr;
	}

	void Chunk::PerformSave()
	{
		bool chunk_file_closed = false; // ;_;

		if (Level* level = ApplicationSingleton::Instance().GetWorld()->GetActiveLevel())
		{
			auto& chunk_root_chunk_info = level->GetChunksManager()->GetChunkRootInfo();

			for (const auto& chunk_info : chunk_root_chunk_info->m_ChunksInfoVec)
			{
				if (chunk_info.m_ChunkUUID == m_ChunkInfo->m_ChunkUUID)
				{
					std::filesystem::path chunk_path = "levels";
					chunk_path /= chunk_info.m_RelativePath;

					fs::IFileSystem* data_fs = ApplicationSingleton::Instance().GetEngineController().GetFilesystemManager()->Get("data");

					if (std::shared_ptr<fs::IFile> chunk_file = data_fs->OpenFile(chunk_path.string(), fs::io::OpenMode::Out | fs::io::OpenMode::Binary))
					{
						chunk_file->Seek(0, fs::io::Origin::Begin);
						chunk_file->SerializeBinary(m_ChunkInfo);
					}
				}
			}
		}
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

		const std::string chunk_file_name = Random::GenerateRandomString(16) + fs::IFile::TypeToStringExt(fs::IFile::EType::Data_LevelChunk);

		fs::IFileSystem* data_fs = ApplicationSingleton::Instance().GetEngineController().GetFilesystemManager()->Get("data");

		std::filesystem::path chunk_file_path = "levels";
		chunk_file_path /= chunk_file_name;

		if (!data_fs->FileExists(chunk_file_path.string()))
		{
			data_fs->CreateFile(chunk_file_path.string(), fs::IFile::EType::Data_LevelChunk);
		}

		std::filesystem::path chunk_root_file_path = "levels";
		chunk_root_file_path /= _lvl_name + fs::IFile::TypeToStringExt(fs::IFile::EType::Data_LevelRootChunk);

		if (!data_fs->FileExists(chunk_root_file_path.string()))
		{
			data_fs->CreateFile(chunk_root_file_path.string(), fs::IFile::EType::Data_LevelRootChunk);
		}

		bool chunk_file_closed = false;
		bool chunk_file_serialized = false;

		const sf::IntRect chunk_area(_x, _y, _width_pixels, _height_pixels);
		std::unique_ptr<Chunk> new_chunk = std::make_unique<Chunk>(chunk_area);

		if (std::shared_ptr<fs::IFile> chunk_file = data_fs->OpenFile(chunk_file_path.string(), fs::io::OpenMode::Out | fs::io::OpenMode::Binary))
		{
			auto& chunk_info = new_chunk->GetChunkInfo();
			chunk_info->m_ChunkUUID = new_chunk_uuid;

			Serializable::Binary::TextureLayerInfo background_layer_zero;
			background_layer_zero.m_LayerIndex = 0;

			const unsigned int final_x = _x + _width_pixels;
			const unsigned int final_y = _y + _height_pixels;

			for (unsigned int i = _x; i < final_x; i += _tile_size)
			{
				for (unsigned int j = _y; j < final_y; j += _tile_size)
				{
					Serializable::Binary::TextureTileInfo tile;
					tile.m_TilesetColumn = 0;
					tile.m_TilesetRow = 0;
					tile.m_TilePositionX = i;
					tile.m_TilePositionY = j;
					background_layer_zero.m_Tiles.push_back(tile);
				}
			}

			Serializable::Binary::TextureLayerInfo foreground_layer_zero;
			foreground_layer_zero.m_LayerIndex = 0;

			for (unsigned int i = _x; i < final_x; i += _tile_size)
			{
				for (unsigned int j = _y; j < final_y; j += _tile_size)
				{
					Serializable::Binary::TextureTileInfo tile;
					tile.m_TilesetColumn = 0;
					tile.m_TilesetRow = 0;
					tile.m_TilePositionX = i;
					tile.m_TilePositionY = j;
					foreground_layer_zero.m_Tiles.push_back(tile);
				}
			}

			
			chunk_info->m_BackgroundTileLayers.push_back(background_layer_zero);
			chunk_info->m_ForegroundTileLayers.push_back(foreground_layer_zero);

			chunk_file->SerializeBinary(chunk_info);

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

		m_ChunkRootInfo->m_ChunksInfoVec.push_back(chunkroot_chunk_info);

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
			m_Chunks.push_back(std::move(new_chunk));

			return true;
		}
		else [[unlikely]]
			{
				new_chunk.reset();

				return false;
			}

	}

	void ChunksManager::PushChunk(std::unique_ptr<Chunk>&& _chunk)
	{
		m_Chunks.push_back(std::move(_chunk));
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