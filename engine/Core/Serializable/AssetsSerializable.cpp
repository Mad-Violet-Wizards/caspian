#include "engine/pch.hpp"

#include "AssetsSerializable.hpp"

Serializable::Binary::TilesetInfo::TilesetInfo(const TilesetInfo& _other)
{
	m_TilesetUUID = _other.m_TilesetUUID;
	m_TilesetPath = _other.m_TilesetPath;
	m_TilesetName = _other.m_TilesetName;
	m_TileWidth = _other.m_TileWidth;
	m_TileHeight = _other.m_TileHeight;
}

Serializable::Binary::TilesetInfo::TilesetInfo(TilesetInfo&& _other) noexcept
{
	std::swap(m_TilesetUUID, _other.m_TilesetUUID);
	std::swap(m_TilesetPath, _other.m_TilesetPath);
	std::swap(m_TilesetName, _other.m_TilesetName);
	std::swap(m_TileWidth, _other.m_TileWidth);
	std::swap(m_TileHeight, _other.m_TileHeight);
}
