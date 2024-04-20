#pragma once

#include "engine/Core/IComponent.hpp"

struct IntersectionResult
{
	bool m_Collided = false;
	const sf::FloatRect* m_Other = nullptr;
};

class C_InterfaceCollidable : public IComponent
{
public:
	C_InterfaceCollidable(GameObject* _owner);
	virtual ~C_InterfaceCollidable() = default;

	void Update([[maybe_unused]] float _deltaTime) override {}
	void LateUpdate([[maybe_unused]] float _deltaTime) override {}

	virtual IntersectionResult Intersects(const std::shared_ptr<C_InterfaceCollidable> _other) = 0;
	virtual void FixDistance(const IntersectionResult& _manifold) = 0;

	void SetOffset(const sf::Vector2f& _offset);
	void SetOffset(float x, float y);

protected:

	sf::Vector2f m_Offset;
};