#include "engine/pch.hpp"

#include "C_Velocity.hpp"

C_Velocity::C_Velocity(GameObject* _owner)
	: IComponent(_owner)
	, m_Velocity(0.f, 0.f)
{

}

void C_Velocity::Update(float _dt)
{
	m_Owner->GetComponent<C_Transform>()->AddPosition(m_Velocity * _dt);
}

void C_Velocity::Set(const sf::Vector2f& _velocity)
{
	m_Velocity = _velocity;
}

void C_Velocity::Set(float _x, float _y)
{
	m_Velocity.x = _x;
	m_Velocity.y = _y;
}

const sf::Vector2f& C_Velocity::Get() const
{
	return m_Velocity;
}
