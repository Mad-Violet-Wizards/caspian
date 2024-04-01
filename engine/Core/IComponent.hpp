#pragma once

class Window;
class GameObject;

class IComponent
{
	public:

		IComponent(GameObject* owner) : m_Owner{ owner } { }

		virtual void Update(float deltaTime) = 0;
		virtual void LateUpdate(float deltaTime) = 0;

	public:

		GameObject* m_Owner;

};