#pragma once

class IComponentOnCollision
{
public:

	virtual ~IComponentOnCollision() = default;
	virtual void OnCollisionEnter() {};
	virtual void OnCollisionStay() {};
	virtual void OnCollisionExit() {};

};