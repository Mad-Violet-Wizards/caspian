#include "engine/pch.hpp"

#include "GameObjectCollection.hpp"

#include "Components/C_Tags.hpp"
#include "Components/C_Transform.hpp"
#include "Components/C_Sprite.hpp"

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
			if (go_tags_component_sPtr->HasTag(ETag::Physics_Dynamic) || go_tags_component_sPtr->HasTag(ETag::Physics_Static))
			{
				auto collidable_game_object_iter = std::find(m_CollidableGameObjects.begin(), m_CollidableGameObjects.end(), game_object_to_remove);

				if (collidable_game_object_iter != m_CollidableGameObjects.end())
				{
					m_CollidableGameObjects.erase(collidable_game_object_iter);
				}
			}

			if (go_tags_component_sPtr->HasTag(ETag::Drawable))
			{
				m_DrawableGameObjects.Remove(game_object_to_remove);
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

	if (m_DebugRenderCollidables)
	{
		auto sprite_component_sPtr = new_game_object->AddComponent<C_Sprite>();
		sprite_component_sPtr->SetTexture(ApplicationSingleton::Instance().GetEngineController().GetAssetsStorage()->GetEmptyTexture());
		sprite_component_sPtr->SetTextureRect(sf::IntRect(0, 0, _collision_rect_info.m_Rect.width, _collision_rect_info.m_Rect.height));
	}

	m_GameObjects.push_back(new_game_object);
	m_CollidableGameObjects.push_back(new_game_object);
}

void GameObjectCollection::ConstructNew(const Serializable::Binary::TextureTileInfo& _texture_tile_info, ETag _drawable_type, unsigned int _active_level_tiles_size, unsigned int _tile_layer)
{
	GameObject* new_game_object = new GameObject();
	auto tag_component_sPtr = new_game_object->AddComponent<C_Tags>(ETag::Drawable);
	tag_component_sPtr->AddTag(_drawable_type);
	auto transform_component_sPtr = new_game_object->AddComponent<C_Transform>();
	transform_component_sPtr->SetX(_texture_tile_info.m_TilePositionX);
	transform_component_sPtr->SetY(_texture_tile_info.m_TilePositionY);
	auto sprite_component_sPtr = new_game_object->AddComponent<C_Sprite>();

	Assets::TilemapStorage* tilemap_storage = ApplicationSingleton::Instance().GetEngineController().GetAssetsStorage()->GetTilemapStorage();

	const Serializable::Binary::TilesetInfo& tileset_info = tilemap_storage->FindTilesetInfo(_texture_tile_info.m_TilesetUUID);

	sprite_component_sPtr->SetTexture(ApplicationSingleton::Instance().GetEngineController().GetAssetsStorage()->GetConstTexture(tileset_info.m_TilesetPath));

	const sf::IntRect texture_rect(_texture_tile_info.m_TilesetColumn, _texture_tile_info.m_TilesetRow, _active_level_tiles_size, _active_level_tiles_size);
	sprite_component_sPtr->SetTextureRect(texture_rect);
	sprite_component_sPtr->SetDrawableType(_drawable_type);
	sprite_component_sPtr->SetLayerIndex(_tile_layer);

	m_GameObjects.push_back(new_game_object);
	m_DrawableGameObjects.Add(new_game_object);
}

const TileIndex GameObjectCollection::CalculateTileIndex(const float _x, const float _y) const
{
	return m_DrawableGameObjects.CalculateTileIndex(_x, _y);
}

void GameObjectCollection::SetRenderCollidables(bool _state)
{
	if (m_DebugRenderCollidables && _state == false)
	{
		// We leave the sprites unchanged, as they are already set to the correct texture anyway.
		m_DebugRenderCollidables = false;
	}
	if (!m_DebugRenderCollidables && _state == true)
	{
		for (GameObject* game_object : m_CollidableGameObjects)
		{
			auto sprite_component_sPtr = game_object->AddComponent<C_Sprite>();
			sprite_component_sPtr->SetTexture(ApplicationSingleton::Instance().GetEngineController().GetAssetsStorage()->GetEmptyTexture());
			const unsigned int tile_size = ApplicationSingleton::Instance().GetWorld()->GetActiveLevel()->GetTilesSize();
			const sf::IntRect texture_rect(0, 0, tile_size, tile_size);
			sprite_component_sPtr->SetTextureRect(texture_rect);
		}
		m_DebugRenderCollidables = true;
	}
}

///////////////////////////////////////////////////////////
// DRAWABLES
///////////////////////////////////////////////////////////
SpatialHashGridBucket::SpatialHashGridBucket(unsigned int _init_buckets_capacity /* = 8*/)
{
}

///////////////////////////////////////////////////////////
SpatialHashGridBucket::~SpatialHashGridBucket()
{

}

///////////////////////////////////////////////////////////
void SpatialHashGridBucket::Add(GameObject* _game_object)
{
	if (_game_object->GetComponent<C_Tags>()->HasTag(ETag::Drawable_Background))
	{
		m_BackgroundGameObjects.push_back(_game_object);

		if (ApplicationSingleton::Instance().GetEngineController().GetGameObjectStorage()->GetSortDrawablesOnDirtyFlag())
			Sort(ETag::Drawable_Background);
	}

	if (_game_object->GetComponent<C_Tags>()->HasTag(ETag::Drawable_Entity))
	{
		m_EntityGameObjects.push_back(_game_object);

		if (ApplicationSingleton::Instance().GetEngineController().GetGameObjectStorage()->GetSortDrawablesOnDirtyFlag())
			Sort(ETag::Drawable_Entity);
	}

	if (_game_object->GetComponent<C_Tags>()->HasTag(ETag::Drawable_Foreground))
	{
		m_ForegroundGameObjects.push_back(_game_object);

		if (ApplicationSingleton::Instance().GetEngineController().GetGameObjectStorage()->GetSortDrawablesOnDirtyFlag())
			Sort(ETag::Drawable_Foreground);
	}
}

///////////////////////////////////////////////////////////
void SpatialHashGridBucket::Remove(GameObject* _game_object)
{
	if (_game_object->GetComponent<C_Tags>()->HasTag(ETag::Drawable_Background))
	{
		auto drawable_object_iter = std::find(m_BackgroundGameObjects.begin(), m_BackgroundGameObjects.end(), _game_object);

		if (drawable_object_iter != m_BackgroundGameObjects.end())
		{
			m_BackgroundGameObjects.erase(drawable_object_iter);
		}

		if (ApplicationSingleton::Instance().GetEngineController().GetGameObjectStorage()->GetSortDrawablesOnDirtyFlag())
			Sort(ETag::Drawable_Background);
	}

	if (_game_object->GetComponent<C_Tags>()->HasTag(ETag::Drawable_Entity))
	{
		auto drawable_object_iter = std::find(m_EntityGameObjects.begin(), m_EntityGameObjects.end(), _game_object);

		if (drawable_object_iter != m_EntityGameObjects.end())
		{
			m_EntityGameObjects.erase(drawable_object_iter);
		}

		if (ApplicationSingleton::Instance().GetEngineController().GetGameObjectStorage()->GetSortDrawablesOnDirtyFlag())
			Sort(ETag::Drawable_Entity);
	}

	if (_game_object->GetComponent<C_Tags>()->HasTag(ETag::Drawable_Foreground))
	{
		auto drawable_object_iter = std::find(m_ForegroundGameObjects.begin(), m_ForegroundGameObjects.end(), _game_object);

		if (drawable_object_iter != m_ForegroundGameObjects.end())
		{
			m_ForegroundGameObjects.erase(drawable_object_iter);
		}

		if (ApplicationSingleton::Instance().GetEngineController().GetGameObjectStorage()->GetSortDrawablesOnDirtyFlag())
			Sort(ETag::Drawable_Foreground);
	}
}

bool SpatialHashGridBucket::Contains(GameObject* _game_object, ETag _drawable_type) const
{
	switch (_drawable_type)
	{
		case ETag::Drawable_Background:
		{
			auto drawable_object_iter = std::find(m_BackgroundGameObjects.begin(), m_BackgroundGameObjects.end(), _game_object);
			if (drawable_object_iter != m_BackgroundGameObjects.end())
				return true;
			break;
		}
		case ETag::Drawable_Entity:
		{
			auto drawable_object_iter = std::find(m_EntityGameObjects.begin(), m_EntityGameObjects.end(), _game_object);
			if (drawable_object_iter != m_EntityGameObjects.end())
				return true;
			break;
		}
		case ETag::Drawable_Foreground:
		{
			auto drawable_object_iter = std::find(m_ForegroundGameObjects.begin(), m_ForegroundGameObjects.end(), _game_object);
			if (drawable_object_iter != m_ForegroundGameObjects.end())
				return true;
			break;
		}
	}

	return false;
}

void SpatialHashGridBucket::Sort(ETag _drawable_type)
{
	auto q_sort_lambda = [](const void* _a, const void* _b) -> int
		{
			if (_a != nullptr && _b != nullptr)
			{
			GameObject* a = *(GameObject**)_a;
			GameObject* b = *(GameObject**)_b;

			auto aGo_sprite_component_Sptr = a->GetComponent<C_Sprite>();
			auto bGo_sprite_component_Sptr = b->GetComponent<C_Sprite>();

			if (aGo_sprite_component_Sptr != nullptr && bGo_sprite_component_Sptr != nullptr)
			{
				return aGo_sprite_component_Sptr->GetLayerIndex() < bGo_sprite_component_Sptr->GetLayerIndex();
			}
		}

		return 0;
	};

	switch (_drawable_type)
	{
		case ETag::Drawable_Background:
		{
			std::qsort(m_BackgroundGameObjects.data(), m_BackgroundGameObjects.size(), sizeof(GameObject*), q_sort_lambda);
			break;
		}
		case ETag::Drawable_Entity:
		{
			std::qsort(m_EntityGameObjects.data(), m_EntityGameObjects.size(), sizeof(GameObject*), q_sort_lambda);
			break;
		}
		case ETag::Drawable_Foreground:
		{
			std::qsort(m_ForegroundGameObjects.data(), m_ForegroundGameObjects.size(), sizeof(GameObject*), q_sort_lambda);
			break;
		}
	}
}

void SpatialHashGrid::Add(GameObject* _game_object)
{
	const TileIndex tile_index = CalculateTileIndex(_game_object);
	m_Grid[tile_index].Add(_game_object);

}

void SpatialHashGrid::Remove(GameObject* _game_object)
{
	const TileIndex tile_index = CalculateTileIndex(_game_object);
	m_Grid[tile_index].Remove(_game_object);
}

const SpatialHashGridBucket& SpatialHashGrid::GetConstRefToBucket(const TileIndex& _bucket_index) const
{
	return m_Grid.at(_bucket_index).GetConstRef();
}

SpatialHashGridBucket& SpatialHashGrid::GetRefToBucket(const TileIndex& _bucket_index)
{
	if (m_Grid.find(_bucket_index) == m_Grid.end())
	{
		SpatialHashGridBucket new_bucket = SpatialHashGridBucket();
		m_Grid.insert({ _bucket_index, new_bucket });
	}

	return m_Grid.at(_bucket_index).GetRef();
}

const TileIndex SpatialHashGrid::CalculateTileIndex(GameObject* _game_object) const
{
	if (_game_object)
	{
		const auto transform_component_sPtr = _game_object->GetComponent<C_Transform>();

		if (transform_component_sPtr)
		{
			const sf::Vector2f& game_object_position = transform_component_sPtr->GetPosition();

			const auto tiles_size = ApplicationSingleton::Instance().GetWorld()->GetActiveLevel()->GetTilesSize();

			const unsigned int tile_x = static_cast<unsigned int>(game_object_position.x / (tiles_size * 8));
			const unsigned int tile_y = static_cast<unsigned int>(game_object_position.y / (tiles_size * 8));

			return { tile_x, tile_y };
		}
	}
}

const TileIndex SpatialHashGrid::CalculateTileIndex(const float _x, const float _y) const
{
	const auto tiles_size = ApplicationSingleton::Instance().GetWorld()->GetActiveLevel()->GetTilesSize();

	return { static_cast<unsigned int>(_x / (tiles_size * 8)), static_cast<unsigned int>(_y / (tiles_size * 8)) };
}
