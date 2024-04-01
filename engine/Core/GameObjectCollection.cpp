#include "engine/pch.hpp"

#include "GameObjectCollection.hpp"

#include "Components/C_Tags.hpp"
#include "Components/C_Transform.hpp"

GameObjectCollection::~GameObjectCollection()
{
	for (GameObject* game_object : m_GameObjects)
	{
		delete game_object;
		game_object = nullptr;
	}

	m_GameObjects.clear();
	m_CollidableGameObjects.clear();
}

void GameObjectCollection::Update(float _dt)
{
	for (GameObject* game_object : m_GameObjects)
	{
		if (game_object->QueuedForRemoval())
		{
			m_GameObjectsToRemove.push(game_object);
			continue;
		}

		game_object->Update(_dt);
	}
}

void GameObjectCollection::LateUpdate(float _dt)
{
	ProcessQueuedForRemoval();

	for (GameObject* game_object : m_GameObjects)
	{
		game_object->LateUpdate(_dt);
	}
}

void GameObjectCollection::ProcessQueuedForRemoval()
{
	while (!m_GameObjectsToRemove.empty())
	{
		GameObject* game_object_to_remove = m_GameObjectsToRemove.front();

		if (auto go_tags_component_sPtr = game_object_to_remove->GetComponent<C_Tags>())
		{
			if (go_tags_component_sPtr->HasTag(ETag::Physics_Dynamic | ETag::Physics_Static))
			{
				auto collidable_game_object_iter = std::find(m_CollidableGameObjects.begin(), m_CollidableGameObjects.end(), game_object_to_remove);

				if (collidable_game_object_iter != m_CollidableGameObjects.end())
				{
					m_CollidableGameObjects.erase(collidable_game_object_iter);
				}
			}
		}

		auto game_object_to_remove_iter = std::find(m_GameObjects.begin(), m_GameObjects.end(), game_object_to_remove);

		if (game_object_to_remove_iter != m_GameObjects.end())
		{
			m_GameObjects.erase(game_object_to_remove_iter);
		}

		delete game_object_to_remove;
		game_object_to_remove = nullptr;
		m_GameObjectsToRemove.pop();
	}
}

void GameObjectCollection::ConstructDebugPlayer()
{
}

void GameObjectCollection::ConstructNew(const Serializable::Binary::CollisionRectInfo& _collision_rect_info)
{
	GameObject* new_game_object = new GameObject();
	auto tag_component_sPtr = new_game_object->AddComponent<C_Tags>(ETag::Physics_Static);
	auto transform_component_sPtr = new_game_object->AddComponent<C_Transform>();
	transform_component_sPtr->SetX(_collision_rect_info.m_Rect.left);
	transform_component_sPtr->SetY(_collision_rect_info.m_Rect.top);

	m_GameObjects.push_back(new_game_object);
	m_CollidableGameObjects.push_back(new_game_object);
}
