#include "engine/pch.hpp"

#include "C_KeyboardMovement.hpp"

C_KeyboardMovement::C_KeyboardMovement(GameObject* _owner)
	: IComponent(_owner)
	, m_MovementSpeed(250.f)
{
	m_p_KeyboardInput = ApplicationSingleton::Instance().GetEngineController().GetKeyboardInputController();
	m_sPtrVelocity = m_Owner->GetComponent<C_Velocity>();
}

void C_KeyboardMovement::Update(float _dt)
{
	float ySpeed = 0.f;
	if (m_p_KeyboardInput->IsKeyPressed(ESupportedKey::W))
	{
		ySpeed = -m_MovementSpeed;
	}

	if (m_p_KeyboardInput->IsKeyPressed(ESupportedKey::S))
	{
		ySpeed = m_MovementSpeed;
	}


	float xSpeed = 0.f;
	if (m_p_KeyboardInput->IsKeyPressed(ESupportedKey::A))
	{
		xSpeed = -m_MovementSpeed;
	}

	if (m_p_KeyboardInput->IsKeyPressed(ESupportedKey::D))
	{
		xSpeed = m_MovementSpeed;
	}

	m_sPtrVelocity->Set(xSpeed, ySpeed);
}

void C_KeyboardMovement::SetMovementSpeed(float _movementSpeed)
{
	m_MovementSpeed = _movementSpeed;
}
