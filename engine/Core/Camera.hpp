#pragma once

class Camera
{
	public:

		Camera();
		~Camera() = default;

		void Update(float _dt);

		void SetFreeCamera(bool _freecam);
		bool IsFreeCamera() const;

		void SetPosition(const sf::Vector2f& _pos);
		void SetViewport(const sf::Vector2f& _viewport);

		void AttachToGameObject(GameObject* game_object);

		// We relay that programmer will provide new position for camera if detached from game object is called.
		void DetachFromGameObject();

	private:

		GameObject* m_OwningGameObject;

		bool m_FreeCamera;

		sf::Vector2f m_Position;
		sf::Vector2f m_Viewport;
};