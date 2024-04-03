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
			if (go_tags_component_sPtr->HasTag(ETag::Physics_Dynamic | ETag::Physics_Static))
			{
				auto collidable_game_object_iter = std::find(m_CollidableGameObjects.begin(), m_CollidableGameObjects.end(), game_object_to_remove);

				if (collidable_game_object_iter != m_CollidableGameObjects.end())
				{
					m_CollidableGameObjects.erase(collidable_game_object_iter);
				}
			}

			if (go_tags_component_sPtr->HasTag(ETag::Drawable))
			{
				auto drawable_object_iter = m_DrawableGameObjects.find(game_object_to_remove);

				if (drawable_object_iter != m_DrawableGameObjects.end(game_object_to_remove))
				{
					m_DrawableGameObjects.Erase(drawable_object_iter);
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

void GameObjectCollection::ConstructNew(const Serializable::Binary::TextureTileInfo& _texture_tile_info, unsigned int _active_level_tiles_size, unsigned int _tile_layer)
{
	GameObject* new_game_object = new GameObject();
	auto tag_component_sPtr = new_game_object->AddComponent<C_Tags>(ETag::Drawable);
	auto transform_component_sPtr = new_game_object->AddComponent<C_Transform>();
	transform_component_sPtr->SetX(_texture_tile_info.m_TilePositionX);
	transform_component_sPtr->SetY(_texture_tile_info.m_TilePositionY);
	auto sprite_component_sPtr = new_game_object->AddComponent<C_Sprite>();


	Assets::TilemapStorage* tilemap_storage = ApplicationSingleton::Instance().GetEngineController().GetAssetsStorage()->GetTilemapStorage();

	const Serializable::Binary::TilesetInfo& tileset_info = tilemap_storage->FindTilesetInfo(_texture_tile_info.m_TilesetUUID);

	sprite_component_sPtr->SetTexture(ApplicationSingleton::Instance().GetEngineController().GetAssetsStorage()->GetConstTexture(tileset_info.m_TilesetPath));

	const sf::IntRect texture_rect(_texture_tile_info.m_TilesetColumn, _texture_tile_info.m_TilesetRow, _active_level_tiles_size, _active_level_tiles_size);
	sprite_component_sPtr->SetTextureRect(texture_rect);
	sprite_component_sPtr->SetLayer(_tile_layer);

	m_GameObjects.push_back(new_game_object);
	m_DrawableGameObjects.Add(new_game_object);
}

const TileIndex GameObjectCollection::CalculateTileIndex(const float _x, const float _y) const
{
	return m_DrawableGameObjects.CalculateTileIndex(_x, _y);
}

///////////////////////////////////////////////////////////
// DRAWABLES
///////////////////////////////////////////////////////////
SpatialHashGridBucket::SpatialHashGridBucket()
{

}

///////////////////////////////////////////////////////////
SpatialHashGridBucket::~SpatialHashGridBucket()
{

}

///////////////////////////////////////////////////////////
void SpatialHashGridBucket::Add(GameObject* _game_object)
{
	m_GameObjects.push_back(_game_object);

	if (ApplicationSingleton::Instance().GetEngineController().GetGameObjectStorage()->GetSortDrawablesOnDirtyFlag())
		Sort();
}

///////////////////////////////////////////////////////////
void SpatialHashGridBucket::Remove(GameObject* _game_object)
{
	auto game_object_iter = std::find(m_GameObjects.begin(), m_GameObjects.end(), _game_object);

	if (game_object_iter != m_GameObjects.end())
	{
		m_GameObjects.erase(game_object_iter);

		if (ApplicationSingleton::Instance().GetEngineController().GetGameObjectStorage()->GetSortDrawablesOnDirtyFlag())
			Sort();
	}
}

std::vector<GameObject*>::iterator SpatialHashGridBucket::find(GameObject* _game_object)
{
	return std::find(m_GameObjects.begin(), m_GameObjects.end(), _game_object);
}

std::vector<GameObject*>::iterator SpatialHashGridBucket::erase(std::vector<GameObject*>::iterator _it)
{
	auto it = m_GameObjects.erase(_it);
	Sort();
	return it;
}

void SpatialHashGridBucket::Sort()
{
	std::qsort(m_GameObjects.data(), m_GameObjects.size(), sizeof(GameObject*), [](const void* _a, const void* _b) -> int
		{
			if (_a != nullptr && _b != nullptr)
			{
				GameObject* a = *(GameObject**)_a;
				GameObject* b = *(GameObject**)_b;

				auto aGo_sprite_component_Sptr = a->GetComponent<C_Sprite>();
				auto bGo_sprite_component_Sptr = b->GetComponent<C_Sprite>();

				if (aGo_sprite_component_Sptr != nullptr && bGo_sprite_component_Sptr != nullptr)
				{
					return aGo_sprite_component_Sptr->GetLayer() > bGo_sprite_component_Sptr->GetLayer();
				}
			}
		});
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

const std::vector<GameObject*>& SpatialHashGrid::GetConstRefToBucket(const TileIndex& _bucket_index) const
{
	return m_Grid.at(_bucket_index).GetConstRef();
}

std::vector<GameObject*>& SpatialHashGrid::GetRefToBucket(const TileIndex& _bucket_index)
{
	return m_Grid[_bucket_index].GetRef();
}

std::vector<GameObject*>::iterator SpatialHashGrid::begin(const TileIndex& _bucket_index)
{
	return m_Grid[_bucket_index].begin();
}

std::vector<GameObject*>::iterator SpatialHashGrid::end(const TileIndex& _bucket_index)
{
	return m_Grid[_bucket_index].end();
}

std::vector<GameObject*>::iterator SpatialHashGrid::find(GameObject* _game_object)
{
	const TileIndex tile_index = CalculateTileIndex(_game_object);
	return m_Grid[tile_index].find(_game_object);
}

std::vector<GameObject*>::iterator SpatialHashGrid::end(GameObject* _game_object)
{
	const TileIndex tile_index = CalculateTileIndex(_game_object);
	return m_Grid[tile_index].end();
}

void SpatialHashGrid::Erase(std::vector<GameObject*>::iterator _it)
{
	const TileIndex tile_index = CalculateTileIndex(*_it);
	m_Grid[tile_index].erase(_it);
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
