#include "engine/pch.hpp"

#include "C_Transform.hpp"

C_Transform::C_Transform(GameObject* owner)
	: IComponent(owner)
	, m_Position(0, 0)
	, m_Static(false)
{

}

void C_Transform::SetPosition(float x, float y)
{
	m_LastFramePosition = m_Position;
	m_Position.x = x;
	m_Position.y = y;
}

void C_Transform::AddPosition(float x, float y)
{
	m_LastFramePosition = m_Position;
	m_Position.x += x;
	m_Position.y += y;
}

void C_Transform::AddPosition(const sf::Vector2f& _pos)
{
	m_LastFramePosition = m_Position;
	m_Position += _pos;
}

void C_Transform::SetX(float x)
{
	m_LastFramePosition = m_Position;
	m_Position.x = x;
}

void C_Transform::SetY(float y)
{
	m_LastFramePosition = m_Position;
	m_Position.y = y;
}

void C_Transform::AddX(float x)
{
	m_LastFramePosition = m_Position;
	m_Position.x += x;
}

void C_Transform::AddY(float y)
{
	m_LastFramePosition = m_Position;
	m_Position.y += y;
}

void C_Transform::SetStatic(bool _static)
{
	m_Static = _static;
}

bool C_Transform::IsStatic() const
{
	return m_Static;
}

const sf::Vector2f& C_Transform::GetPosition() const
{
	return m_Position;
}

const sf::Vector2f& C_Transform::GetLastFramePosition() const
{
	return m_LastFramePosition;
}
