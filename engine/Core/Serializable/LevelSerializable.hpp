#pragma once
#include "engine/Filesystem/FilesystemMark.hpp"

#include <cereal/archives/portable_binary.hpp>
#include <cereal/archives/json.hpp>

#include "Serializable.hpp"

namespace Serializable
{
	namespace JSON
	{
		class LevelInfo : public ISerializable::JSON
		{
			public:

				LevelInfo() = default;
				LevelInfo(const std::string& _level_name,
									const std::string& _chunk_root_file,
									unsigned int _tile_width,
									unsigned int _tile_height)
					: m_LevelName{ _level_name }
					, m_ChunkRootFile{ _chunk_root_file }
					, m_TileWidth{ _tile_width }
					, m_TileHeight{ _tile_height }
					, m_FsMark{ fs::EFilesystemMarkToHash(fs::EFilesystemMark::Level) }
				{
				}

			public:

				std::string m_LevelName;
				std::string m_ChunkRootFile;
				unsigned int m_TileWidth;
				unsigned int m_TileHeight;
				unsigned int m_FsMark;

				void dummy() override {}

				template<class Archive>
				void serialize(Archive& ar)
				{
					ar(cereal::make_nvp("Name", m_LevelName), 
						cereal::make_nvp("ChunkRootFile", m_ChunkRootFile),
						cereal::make_nvp("Tile Width", m_TileWidth),
						cereal::make_nvp("Tile Height", m_TileHeight), 
						cereal::make_nvp("Fs Mark", m_FsMark));
				}
		};

	}

	namespace Binary
	{
		class TextureTileInfo : public ISerializable::Binary
		{
		public:

			TextureTileInfo() = default;

			void dummy() override {}

			Random::UUID m_TilesetUUID;
			Random::UUID m_TileUUID = Random::EMPTY_UUID;
			unsigned int m_TilesetRow;
			unsigned int m_TilesetColumn;
			// Do we need to store the tile pos in the world?

			template<class Archive>
			void serialize(Archive& archive)
			{
				archive(cereal::make_nvp("TilesetUUID", m_TilesetUUID.GetUUID()),
								cereal::make_nvp("TileUUID", m_TileUUID.GetUUID()),
								cereal::make_nvp("TilesetRow", m_TilesetRow),
								cereal::make_nvp("TilesetColumn", m_TilesetColumn));
			}
		};

		class TextureLayerInfo : public ISerializable::Binary
		{
		public:

			TextureLayerInfo() = default;

			void dummy() override {}
			int m_LayerIndex = 0;
			std::vector<TextureTileInfo> m_Tiles;

			template<class Archive>
			void serialize(Archive& archive)
			{
								archive(cereal::make_nvp("LayerIndex", m_LayerIndex),
												cereal::make_nvp("Tiles", m_Tiles));
			}
		};

		class ChunkInfo : public ISerializable::Binary
		{
		public:

			ChunkInfo() = default;

			void dummy() override {}

			Random::UUID m_ChunkUUID; // Same as in ChunkRoot_ChunkInfo
			std::vector<TextureLayerInfo> m_TileLayers;

			template<class Archive>
			void serialize(Archive& archive)
			{
				archive(cereal::make_nvp("UUID", m_ChunkUUID.GetUUID()),
								cereal::make_nvp("TileLayers", m_TileLayers));
			}
		};

		class ChunkRoot_ChunkInfo : public ISerializable::Binary
		{
		public:

			ChunkRoot_ChunkInfo() = default;

			void dummy() override {}

			Random::UUID m_ChunkUUID; // Same as in ChunkInfo.
			std::string m_RelativePath;
			sf::Vector2u m_ChunkPosX;
			sf::Vector2u m_ChunkPosY;

			template<class Archive>
			void serialize(Archive& archive)
			{
				archive(cereal::make_nvp("UUID", m_ChunkUUID.GetUUID()),
					cereal::make_nvp("RelativePath", m_RelativePath),
					cereal::make_nvp("X_START", m_ChunkPosX.x),
					cereal::make_nvp("X_END", m_ChunkPosX.y),
					cereal::make_nvp("Y_START", m_ChunkPosY.x),
					cereal::make_nvp("Y_END", m_ChunkPosY.y));
			}
		};

		class ChunkRootInfo : public ISerializable::Binary
		{
		public:

			ChunkRootInfo() = default;

			std::vector<ChunkRoot_ChunkInfo> m_ChunksInfoVec;

			void dummy() override {}

			template<class Archive>
			void serialize(Archive& archive)
			{
				archive(m_ChunksInfoVec);
			}
		};
	}
}

using namespace Serializable::JSON;
using namespace Serializable::Binary;
using namespace ISerializable;

CEREAL_REGISTER_TYPE(LevelInfo);
CEREAL_REGISTER_POLYMORPHIC_RELATION(JSON, LevelInfo);

CEREAL_REGISTER_TYPE(TextureTileInfo);
CEREAL_REGISTER_POLYMORPHIC_RELATION(Binary, TextureTileInfo);

CEREAL_REGISTER_TYPE(TextureLayerInfo);
CEREAL_REGISTER_POLYMORPHIC_RELATION(Binary, TextureLayerInfo);

CEREAL_REGISTER_TYPE(ChunkInfo);
CEREAL_REGISTER_POLYMORPHIC_RELATION(Binary, ChunkInfo);

CEREAL_REGISTER_TYPE(ChunkRoot_ChunkInfo);
CEREAL_REGISTER_POLYMORPHIC_RELATION(Binary, ChunkRoot_ChunkInfo);

CEREAL_REGISTER_TYPE(ChunkRootInfo);
CEREAL_REGISTER_POLYMORPHIC_RELATION(Binary, ChunkRootInfo);