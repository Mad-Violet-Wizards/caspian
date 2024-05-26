#pragma once

#include "engine/Core/IComponent.hpp"

class C_Velocity : public IComponent
{
	public:

		C_Velocity(GameObject* _owner);

		void Update(float _dt) override;
		void LateUpdate(float _dt) override {};

		void Set(const sf::Vector2f& _velocity);
		void Set(float _x, float _y);

		const sf::Vector2f& Get() const;


		sf::Vector2f m_Velocity;
};