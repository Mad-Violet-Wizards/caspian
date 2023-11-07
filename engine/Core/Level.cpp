#include "engine/pch.hpp"

#include "Level.hpp"

//////////////////////////////////////////////////////////////////////////
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


//////////////////////////////////////////////
// LEVEL
void Level::Level::Draw(Window& _window)
{

}


void Level::Level::Update(float _dt)
{

}

//////////////////////////////////////////////
// LAYER
Level::Layer::Layer(size_t width, size_t height)
{

}

//////////////////////////////////////////////
// TILE
Level::Tile::Tile(unsigned int _tileset_id, size_t _tileset_x, size_t _tileset_y, size_t width, size_t height)
{

}

void Level::Tile::Draw(Window& _window)
{

}

//////////////////////////////////////////////
// TILESET
Level::Tileset::Tileset(const std::string& _path)
{

}