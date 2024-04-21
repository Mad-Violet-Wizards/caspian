#pragma once
#include "engine/pch.hpp"
#include "C_InterfaceCollidable.hpp"

C_InterfaceCollidable::C_InterfaceCollidable(GameObject* _owner)
	: IComponent(_owner)
{
}

void C_InterfaceCollidable::SetOffset(const sf::Vector2f& _offset)
{
	m_Offset = _offset;
}

void C_InterfaceCollidable::SetOffset(float x, float y)
{
	m_Offset.x = x;
	m_Offset.y = y;
}

