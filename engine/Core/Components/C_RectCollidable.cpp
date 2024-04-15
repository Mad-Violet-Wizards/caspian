#pragma once
#include "engine/pch.hpp"

#include <math.h>
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

	const sf::FloatRect& this_rect = GetRect();
	const sf::FloatRect* other_rect = _manifold.m_Other;

	float x_overlap = this_rect.left + this_rect.width - other_rect->left + other_rect->width;
	float y_overlap = this_rect.top + this_rect.height - other_rect->top + other_rect->height;

	if (fabs(x_overlap) > fabs(y_overlap))
	{
		if (x_overlap > 0)
		{
			transform->AddPosition((other_rect->left + other_rect->width) - this_rect.left, 0);
		}
		else
		{
			transform->AddPosition(-((this_rect.left + this_rect.width) - other_rect->left), 0);
		}
	}
	else
	{
		if (y_overlap > 0)
		{
			transform->AddPosition(0, (other_rect->top + other_rect->height) - this_rect.top);
		}
		else
		{
			transform->AddPosition(0, -((this_rect.top + this_rect.height) - other_rect->top));
		}
	}
}

void C_RectCollidable::SetRect(const sf::FloatRect& _rect)
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
