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
					const std::string& _collisions_info_file,
					unsigned int _tile_size)
					: m_LevelName{ _level_name }
					, m_ChunkRootFile{ _chunk_root_file }
					, m_CollisionsInfoFile{ _collisions_info_file }
					, m_TileSize{ _tile_size }
					, m_FsMark{ fs::EFilesystemMarkToHash(fs::EFilesystemMark::Level) }
				{
				}

		public:

			std::string m_LevelName;
			std::string m_ChunkRootFile;
			std::string m_CollisionsInfoFile;
			unsigned int m_TileSize;
			unsigned int m_FsMark;

			void dummy() override {}

			template<class Archive>
			void serialize(Archive& ar)
			{
				ar(cereal::make_nvp("Name", m_LevelName),
					cereal::make_nvp("ChunkRootFile", m_ChunkRootFile),
					cereal::make_nvp("CollisionsInfoFile", m_CollisionsInfoFile),
					cereal::make_nvp("TileSize", m_TileSize),
					cereal::make_nvp("Fs Mark", m_FsMark));
			}
		};

	}

	namespace Binary
	{
		class CollisionRectInfo : public ISerializable::Binary
		{
		public:
			CollisionRectInfo() = default;
			CollisionRectInfo(const sf::IntRect& _int_rect);
			CollisionRectInfo(const CollisionRectInfo& _other);
			CollisionRectInfo(CollisionRectInfo&& _other) noexcept;

			CollisionRectInfo& operator= (const CollisionRectInfo& _other);
			CollisionRectInfo& operator= (CollisionRectInfo&& _other) noexcept;

			void dummy() override {}

			sf::IntRect m_Rect;

			template<class Archive>
			void serialize(Archive& archive)
			{
				archive(cereal::make_nvp("x", m_Rect.left),
								cereal::make_nvp("y", m_Rect.top),
								cereal::make_nvp("width", m_Rect.width),
								cereal::make_nvp("height", m_Rect.height));
			}
		};

		class CollisionLayerInfo : public ISerializable::Binary
		{
			public:

				CollisionLayerInfo() = default;

				void dummy() override {}

				std::vector<CollisionRectInfo> m_CollisionRects;

				template<class Archive>
				void serialize(Archive& archive)
				{
					archive(cereal::make_nvp("CollisionRects", m_CollisionRects));
				}
		};


		class TextureTileInfo : public ISerializable::Binary
		{
		public:

			TextureTileInfo() = default;
			TextureTileInfo(const TextureTileInfo& _other);
			TextureTileInfo(TextureTileInfo&& _other) noexcept;

			void dummy() override {}

			uint64_t m_TilesetUUID = Random::EMPTY_UUID;
			uint64_t m_TileUUID = Random::EMPTY_UUID;
			unsigned int m_TilesetRow = 0;
			unsigned int m_TilesetColumn = 0;
			unsigned int m_TilePositionX = 0;
			unsigned int m_TilePositionY = 0;

			template<class Archive>
			void serialize(Archive& archive)
			{
				archive(cereal::make_nvp("TilesetUUID", m_TilesetUUID),
								cereal::make_nvp("TileUUID", m_TileUUID),
								cereal::make_nvp("TilesetRow", m_TilesetRow),
								cereal::make_nvp("TilesetColumn", m_TilesetColumn),
								cereal::make_nvp("TilePositionX", m_TilePositionX),
								cereal::make_nvp("TilePositionY", m_TilePositionY));
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

			uint64_t m_ChunkUUID; // Same as in ChunkRoot_ChunkInfo
			std::vector<TextureLayerInfo> m_BackgroundTileLayers;
			std::vector<TextureLayerInfo> m_ForegroundTileLayers;

			template<class Archive>
			void serialize(Archive& archive)
			{
				archive(cereal::make_nvp("UUID", m_ChunkUUID),
								cereal::make_nvp("BackgroundTileLayers", m_BackgroundTileLayers),
								cereal::make_nvp("ForegroundTileLayers", m_ForegroundTileLayers));
			}
		};

		class ChunkRoot_ChunkInfo : public ISerializable::Binary
		{
		public:

			ChunkRoot_ChunkInfo() = default;

			void dummy() override {}

			uint64_t m_ChunkUUID; // Same as in ChunkInfo.
			std::string m_RelativePath;
			sf::Vector2u m_ChunkPosX; // start pos (x) + width
			sf::Vector2u m_ChunkPosY; // start pos (y) + height

			template<class Archive>
			void serialize(Archive& archive)
			{
				archive(cereal::make_nvp("UUID", m_ChunkUUID),
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

CEREAL_REGISTER_TYPE(CollisionRectInfo);
CEREAL_REGISTER_POLYMORPHIC_RELATION(Binary, CollisionRectInfo);

CEREAL_REGISTER_TYPE(CollisionLayerInfo);
CEREAL_REGISTER_POLYMORPHIC_RELATION(Binary, CollisionLayerInfo);