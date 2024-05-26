#pragma once

#include "engine/Core/IComponent.hpp"
#include "C_Velocity.hpp"

enum class EDirection
{
	Up,
	Down,
	Left,
	Right
};

class C_Direction : public IComponent
{
	public:

		C_Direction(GameObject* _owner);

		void Update(float _dt) override;
		void LateUpdate(float _dt) override {};

		EDirection Get() const;

	private:

		std::shared_ptr<C_Velocity> m_sPtrVelocity;

		EDirection m_Direction;
};