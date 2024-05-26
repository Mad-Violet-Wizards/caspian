#include "engine/pch.hpp"

#include "C_Direction.hpp"

C_Direction::C_Direction(GameObject* _owner)
	: IComponent(_owner)
	, m_Direction(EDirection::Down)
{
	m_sPtrVelocity = m_Owner->GetComponent<C_Velocity>();
}

void C_Direction::Update(float _dt)
{
	const sf::Vector2f current_velocity = m_sPtrVelocity->Get();

	if (current_velocity.x == 0.f && current_velocity.y == 0.f)
	{
		return;
	}

	if (current_velocity.x > 0.f)
	{
		m_Direction = EDirection::Right;
	}
	else if (current_velocity.x < 0.f)
	{
		m_Direction = EDirection::Left;
	}
	else if (current_velocity.y > 0.f)
	{
		m_Direction = EDirection::Down;
	}
	else if (current_velocity.y < 0.f)
	{
		m_Direction = EDirection::Up;
	}
}

EDirection C_Direction::Get() const
{
	return m_Direction;
}
