#pragma once

#include "engine/Core/IComponent.hpp"

class C_Transform : public IComponent
{
	public:
		
		C_Transform(GameObject* owner);

		void SetPosition(float x, float y);

		void AddPosition(float x, float y);
		void AddPosition(const sf::Vector2f& _pos);

		void SetX(float x);
		void SetY(float y);

		void AddX(float x);
		void AddY(float y);

		const sf::Vector2f& GetPosition() const;

		virtual void Update([[maybe_unused]] float _dt) override {};
		virtual void LateUpdate([[maybe_unused]] float _dt) override {}

	private:
		
		sf::Vector2f m_Position;
};