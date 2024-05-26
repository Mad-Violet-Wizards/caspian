#pragma once

#include "engine/Core/IComponent.hpp"
#include "engine/Core/Components/C_Direction.hpp"
#include "engine/Core/Components/C_Animatable.hpp"
#include "engine/Core/Components/C_Velocity.hpp"

enum class EMovementAnimationState
{
	Idle,
	Walking
};

class C_MovementAnimation : public IComponent
{
	public:

		C_MovementAnimation(GameObject* _owner);

		void Update(float _dt) override;
		void LateUpdate(float _dt) override {};

		void BindAnimation(EMovementAnimationState _state, EDirection _direction, const std::string& _animationName);

	private:

		std::map<std::pair<EMovementAnimationState, EDirection>, std::string> m_AnimationBindings;

		std::shared_ptr<C_Direction> m_sPtrDirection;
		std::shared_ptr<C_Animatable> m_sPtrAnimatable;
		std::shared_ptr<C_Velocity> m_sPtrVelocity;
};