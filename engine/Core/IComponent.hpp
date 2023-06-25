#pragma once

class Window;
class GameObject;

class IComponent
{
	public:

		IComponent(std::shared_ptr<GameObject> owner) : m_owner{ owner } { }

		virtual void Awake() = 0;
		virtual void Start() = 0;

		virtual void Update(float deltaTime) = 0;
		virtual void LateUpdate(float deltaTime) = 0;
		virtual void Draw(Window& window);

	public:

		std::shared_ptr<GameObject> m_owner;

};