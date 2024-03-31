#pragma once

class Camera
{
	public:

		Camera();
		~Camera() = default;

		void Update(float _dt);

		void SetPosition(const sf::Vector2f& _pos);
		void AddPosition(const sf::Vector2f& _pos);
		const sf::Vector2f& GetPosition() const;

		const sf::View& GetBaseView() const;

		void AttachToGameObject(GameObject* game_object);

		// We relay that programmer will provide new position for camera if detached from game object is called.
		void DetachFromGameObject();

		void SetLevelBounds(const sf::FloatRect& _bounds);

	private:

		void InitializeEventListeners();

	private:

		GameObject* m_OwningGameObject;

		sf::FloatRect m_LevelBounds;

		sf::Vector2f m_Position;

		sf::View m_BaseView;
};