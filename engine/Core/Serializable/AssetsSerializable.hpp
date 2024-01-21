#pragma once
#include "Serializable.hpp"
#include <cereal/types/vector.hpp>
#include <cereal/types/array.hpp>
#include <cereal/types/polymorphic.hpp>
#include <cereal/types/string.hpp>
#include <cereal/archives/portable_binary.hpp>
#include <cereal/archives/json.hpp>

namespace Serializable
{
	namespace Binary
	{
		class TilesetInfo : public ISerializable::Binary
		{
		public:

			TilesetInfo() = default;
			TilesetInfo(const std::string& _path, const std::string& _name, unsigned int _width, unsigned int _height)
				: m_TilesetPath(_path)
				, m_TilesetName(_name)
				, m_TileWidth(_width)
				, m_TileHeight(_height)
			{}

				void dummy() override {}

				Random::UUID m_TilesetUUID;
				std::string m_TilesetPath;
				std::string m_TilesetName;
				unsigned int m_TileWidth;
				unsigned int m_TileHeight;

				template<class Archive>
				void serialize(Archive& archive)
				{
					archive(cereal::make_nvp("UUID", m_TilesetUUID.GetUUID()),
									cereal::make_nvp("Key", m_TilesetPath), 
									cereal::make_nvp("Name", m_TilesetName),
									cereal::make_nvp("Width", m_TileWidth),
									cereal::make_nvp("Height", m_TileHeight));
				}
		};

		class TilesetsInfo : public ISerializable::Binary
		{
			public:
			
				TilesetsInfo() = default;

				std::vector<TilesetInfo> m_Tilesets;

				void dummy() override {}

				template<class Archive>
				void serialize(Archive& archive)
				{
					archive(m_Tilesets);
				}
		};
	}
}

using namespace Serializable::Binary;
using namespace ISerializable;

CEREAL_REGISTER_TYPE(TilesetInfo);
CEREAL_REGISTER_TYPE(TilesetsInfo);

CEREAL_REGISTER_POLYMORPHIC_RELATION(Binary, TilesetInfo);
CEREAL_REGISTER_POLYMORPHIC_RELATION(Binary, TilesetsInfo);
