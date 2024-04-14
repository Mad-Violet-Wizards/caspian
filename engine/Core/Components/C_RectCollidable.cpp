#pragma once
#include "engine/pch.hpp"
#include "C_RectCollidable.hpp"

C_RectCollidable::C_RectCollidable(GameObject* _owner)
	: C_InterfaceCollidable(_owner)
{
}

C_RectCollidable::~C_RectCollidable()
{

}

IntersectionResult C_RectCollidable::Intersects(const std::shared_ptr<C_InterfaceCollidable> _other)
{
	IntersectionResult result;

	std::shared_ptr<C_RectCollidable> other_box_collider = std::dynamic_pointer_cast<C_RectCollidable>(_other);

	if (other_box_collider)
	{
		if (GetRect().intersects(other_box_collider->GetRect()))
		{
			result.m_Collided = true;
			result.m_Other = &other_box_collider->GetRect();
		}
	}

	return result;
}

void C_RectCollidable::ResolveOverlap(const IntersectionResult& _manifold)
{
	auto transform = m_Owner->GetComponent<C_Transform>();

	if (transform->IsStatic())
		return;

	// CONTINUE HERE.
}

void C_RectCollidable::SetCollidable(const sf::FloatRect& _rect)
{
		m_AABB = _rect;
}

void C_RectCollidable::SetSize(const sf::Vector2f& _size)
{
	m_AABB.width = _size.x;
	m_AABB.height = _size.y;
}

void C_RectCollidable::SetSize(float _width, float _height)
{
	m_AABB.width = _width;
	m_AABB.height = _height;
}

const sf::FloatRect& C_RectCollidable::GetRect()
{
	auto transform = m_Owner->GetComponent<C_Transform>();

	if (transform != nullptr)
	{
		m_AABB.left = transform->GetPosition().x + m_Offset.x;
		m_AABB.top = transform->GetPosition().y + m_Offset.y;
	}

	return m_AABB;
}
