#include "engine/pch.hpp"

#include <vendor/include/nlohmann/json.hpp>
#include "Level.hpp"

/////////////////////////////////////3/////////////////////////////////////
// WORLD
void Level::World::ActivateLevel(const std::string& _level_name)
{

}

void Level::World::DeactivateLevel(const std::string& _level_name)
{

}

void Level::World::SwitchToLevel(const std::string& _level_name)
{

}

void Level::World::Draw()
{

}

void Level::World::Update(float _dt)
{

}

// TODO: Waste of memory & dandlding std::shared_ptr...
void Level::World::PushInitialLevelData(std::shared_ptr<Serializable::JSON::LevelInfo>& _level_info)
{
	m_InitialLevelsData.push_back({ _level_info->m_LevelName, _level_info->m_ChunkRootFile, _level_info->m_TileWidth, _level_info->m_TileHeight });
}
