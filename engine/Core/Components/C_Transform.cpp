#include "engine/pch.hpp"

#include "C_Transform.hpp"

C_Transform::C_Transform(GameObject* owner)
	: IComponent(owner)
	, m_Position(0, 0)
{

}

void C_Transform::SetPosition(float x, float y)
{
	m_Position.x = x;
	m_Position.y = y;
}

void C_Transform::AddPosition(float x, float y)
{
	m_Position.x += x;
	m_Position.y += y;
}

void C_Transform::AddPosition(const sf::Vector2f& _pos)
{
		m_Position += _pos;
}

void C_Transform::SetX(float x)
{
	m_Position.x = x;
}

void C_Transform::SetY(float y)
{
	m_Position.y = y;
}

void C_Transform::AddX(float x)
{
	m_Position.x += x;
}

void C_Transform::AddY(float y)
{
	m_Position.y += y;
}

const sf::Vector2f& C_Transform::GetPosition() const
{
	return m_Position;
}
