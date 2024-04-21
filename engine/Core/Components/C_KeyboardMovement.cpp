#include "engine/pch.hpp"

#include "C_KeyboardMovement.hpp"

C_KeyboardMovement::C_KeyboardMovement(GameObject* _owner)
	: IComponent(_owner)
	, m_MovementSpeed(250.f)
{
	m_p_KeyboardInput = ApplicationSingleton::Instance().GetEngineController().GetKeyboardInputController();
}

void C_KeyboardMovement::Update(float _dt)
{
	const std::shared_ptr<C_Transform> transform = m_Owner->GetComponent<C_Transform>();

	if (m_p_KeyboardInput->IsKeyPressed(ESupportedKey::W))
	{
		transform->AddY(-m_MovementSpeed * _dt);
	}

	if (m_p_KeyboardInput->IsKeyPressed(ESupportedKey::S))
	{
		transform->AddY(m_MovementSpeed * _dt);
	}

	if (m_p_KeyboardInput->IsKeyPressed(ESupportedKey::A))
	{
		transform->AddX(-m_MovementSpeed * _dt);
	}

	if (m_p_KeyboardInput->IsKeyPressed(ESupportedKey::D))
	{
		transform->AddX(m_MovementSpeed * _dt);
	}
}

void C_KeyboardMovement::SetMovementSpeed(float _movementSpeed)
{
	m_MovementSpeed = _movementSpeed;
}
