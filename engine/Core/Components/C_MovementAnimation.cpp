#include "engine/pch.hpp"

#include "C_MovementAnimation.hpp"

C_MovementAnimation::C_MovementAnimation(GameObject* _owner)
	: IComponent(_owner)
{
	m_sPtrVelocity = m_Owner->GetComponent<C_Velocity>();
	m_sPtrDirection = m_Owner->GetComponent<C_Direction>();
	m_sPtrAnimatable = m_Owner->GetComponent<C_Animatable>();
}


void C_MovementAnimation::Update(float _dt)
{
	const sf::Vector2f& curr_velocity = m_sPtrVelocity->Get();
	const EMovementAnimationState state = (curr_velocity.x == 0.f && curr_velocity.y == 0.f) ? EMovementAnimationState::Idle : EMovementAnimationState::Walking;
	const EDirection direction = m_sPtrDirection->Get();

	const std::string& animation_name = m_AnimationBindings.at(std::make_pair(state, direction));
	m_sPtrAnimatable->PlayAnimation(animation_name);
}

void C_MovementAnimation::BindAnimation(EMovementAnimationState _state, EDirection _direction, const std::string& _animationName)
{
	const std::pair pair = std::make_pair(_state, _direction);

	if (m_AnimationBindings.find(pair) != m_AnimationBindings.end())
	{
		return;
	}

	m_AnimationBindings[pair] = _animationName;
}
