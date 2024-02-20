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

		void AddPosition(const sf::Vector2f& _pos);
		void AddViewport(const sf::Vector2f& _viewport);

		const sf::Vector2f& GetPosition() const;
		const sf::Vector2f& GetViewport() const;

		void SetZoom(float _zoom);
		void AddZoom(float _zoom);
		float GetZoom() const;

		const sf::View& GetBaseView() const;

		void AttachToGameObject(GameObject* game_object);

		// We relay that programmer will provide new position for camera if detached from game object is called.
		void DetachFromGameObject();

		void SetLevelBounds(const sf::FloatRect& _bounds);

	private:

		void InitializeEventListeners();

	private:

		GameObject* m_OwningGameObject;

		bool m_FreeCamera;
		float m_Zoom;
		float m_ZoomDelta;
		float m_ZoomSpeedFactor;
		sf::FloatRect m_LevelBounds;

		sf::Vector2f m_Position;
		sf::Vector2f m_Size;

		sf::View m_BaseView;

		std::unique_ptr<Events::Listener> m_WindowResizeListener;
};