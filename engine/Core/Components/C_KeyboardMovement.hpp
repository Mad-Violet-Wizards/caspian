#pragma once

#include "engine/Core/IComponent.hpp"
#include "engine/Core/Components/C_Velocity.hpp"

/////////////////////////////////////
class C_KeyboardMovement : public IComponent
{
public:
	
	C_KeyboardMovement(GameObject* _owner);

	void Update(float _dt) override;
	void LateUpdate(float _dt) override {};

	void SetMovementSpeed(float _movementSpeed);

private:


	std::shared_ptr<C_Velocity> m_sPtrVelocity;
	KeyboardInputController* m_p_KeyboardInput;
	float m_MovementSpeed;

};