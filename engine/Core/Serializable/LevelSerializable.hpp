#pragma once
#include "engine/Filesystem/FilesystemMark.hpp"

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
}

using namespace Serializable::JSON;
using namespace ISerializable;

CEREAL_REGISTER_TYPE(LevelInfo);
CEREAL_REGISTER_POLYMORPHIC_RELATION(JSON, LevelInfo);
