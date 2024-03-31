#include "engine/pch.hpp"

#include "LevelSerializable.hpp"

Serializable::Binary::TextureTileInfo::TextureTileInfo(const TextureTileInfo& _other)
{
	m_TilesetUUID  = _other.m_TilesetUUID;
	m_TileUUID = _other.m_TileUUID;
	m_TilesetRow = _other.m_TilesetRow;
	m_TilesetColumn = _other.m_TilesetColumn;
	m_TilePositionX = _other.m_TilePositionX;
	m_TilePositionY = _other.m_TilePositionY;
}

Serializable::Binary::TextureTileInfo::TextureTileInfo(TextureTileInfo&& _other) noexcept
{
	std::swap(m_TilesetUUID, _other.m_TilesetUUID);
	std::swap(m_TileUUID, _other.m_TileUUID);
	std::swap(m_TilesetRow, _other.m_TilesetRow);
	std::swap(m_TilesetColumn, _other.m_TilesetColumn);
	std::swap(m_TilePositionX, _other.m_TilePositionX);
	std::swap(m_TilePositionY, _other.m_TilePositionY);
}

Serializable::Binary::CollisionRectInfo::CollisionRectInfo(const sf::IntRect& _int_rect)
	: m_Rect(_int_rect)
{
}

Serializable::Binary::CollisionRectInfo::CollisionRectInfo(CollisionRectInfo&& _other) noexcept
{
	m_Rect = std::move(_other.m_Rect);
}

Serializable::Binary::CollisionRectInfo::CollisionRectInfo(const CollisionRectInfo& _other)
{
	m_Rect = _other.m_Rect;
}
