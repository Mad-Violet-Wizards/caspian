#pragma once

class GameObject;

// TODO: Better to create iter begin and end as we almost always have to iterate over the collection.

class GameObjectCollection
{
	public:

		GameObjectCollection() = default;
		~GameObjectCollection();

		void Update(float _dt);
		void LateUpdate(float _dt);

		void ProcessQueuedForRemoval();

		void ConstructDebugPlayer();
		void ConstructNew(const Serializable::Binary::CollisionRectInfo& _collision_rect_info);

		const std::vector<GameObject*>& GetCollidableGameObjects() const { return m_CollidableGameObjects; }

	private:

		std::vector<GameObject*> m_GameObjects;

		std::vector<GameObject*> m_CollidableGameObjects;

		std::queue<GameObject*> m_GameObjectsToRemove; 

		GameObject* m_Player;
};