#pragma once
#include "engine/Core/Components/C_InterfaceCollidable.hpp"

class C_RectCollidable : public C_InterfaceCollidable
{
public:

	C_RectCollidable(GameObject* _owner);
	~C_RectCollidable();

	IntersectionResult Intersects(const std::shared_ptr<C_InterfaceCollidable> _other) override;
	void FixDistance(const IntersectionResult& _manifold) override;

	void SetSize(const sf::Vector2f& _size);
	void SetSize(float _width, float _height);

	void SetRect(const sf::FloatRect& _rect);
	const sf::FloatRect& GetRect();

private:

	sf::FloatRect m_AABB;

};