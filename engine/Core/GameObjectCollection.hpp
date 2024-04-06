#pragma once

#include "engine/Core/Components/C_Tags.hpp"

class GameObject;

class SpatialHashGridBucket
{
	public:

		SpatialHashGridBucket(unsigned int _init_buckets_capacity = 8);
		~SpatialHashGridBucket();

		void Add(GameObject* _game_object);
		void Remove(GameObject* _game_object);

		const SpatialHashGridBucket& GetConstRef() const { return *this; }
		SpatialHashGridBucket& GetRef() { return *this; }
		bool Contains(GameObject* _game_object, ETag _drawable_type) const;
		
		std::vector<GameObject*>::iterator background_objects_begin() { return m_BackgroundGameObjects.begin(); }

		std::vector<GameObject*>::iterator background_objects_end() { return m_BackgroundGameObjects.end(); }

		std::vector<GameObject*>::iterator entity_objects_begin() { return m_EntityGameObjects.begin(); }

		std::vector<GameObject*>::iterator entity_objects_end() { return m_EntityGameObjects.end(); }

		std::vector<GameObject*>::iterator foreground_objects_begin() { return m_ForegroundGameObjects.begin(); }

		std::vector<GameObject*>::iterator foreground_objects_end() { return m_ForegroundGameObjects.end(); }

	private:

		void Sort(ETag _drawable_type);

	private:

		std::vector<GameObject*> m_BackgroundGameObjects;
		std::vector<GameObject*> m_EntityGameObjects;
		std::vector<GameObject*> m_ForegroundGameObjects;
};

struct TileIndexHash
{
	template<class A, class B>
	std::size_t operator()(const std::pair<A, B>& _pair) const
	{
		return std::hash<A>()(_pair.first) ^ std::hash<B>()(_pair.second);
	}
};

using TileIndex = std::pair<int, int>;

// TODO: Better to create iter begin and end as we almost always have to iterate over the collection.
class SpatialHashGrid
{
public:

	SpatialHashGrid() = default;
	~SpatialHashGrid() = default;

	void Add(GameObject* _game_object);
	void Remove(GameObject* _game_object);

	const SpatialHashGridBucket& GetConstRefToBucket(const TileIndex& _bucket_index) const;
	SpatialHashGridBucket& GetRefToBucket(const TileIndex& _bucket_index);

	bool Empty() const { return m_Grid.empty(); }

	const TileIndex CalculateTileIndex(const float _x, const float _y) const;

private:

	const TileIndex CalculateTileIndex(GameObject* _game_object) const;

private:

	std::unordered_map<TileIndex, SpatialHashGridBucket, TileIndexHash> m_Grid;
};


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
		void ConstructNew(const Serializable::Binary::TextureTileInfo& _texture_tile_info, ETag _drawable_type, unsigned int _active_level_tiles_size, unsigned int _tile_layer);

		const std::vector<GameObject*>& GetCollidableGameObjects() const { return m_CollidableGameObjects; }

		void SortDrawablesOnDirtyFlag(bool _state) { m_SortDrawablesOnDirtyFlag = _state; }
		bool GetSortDrawablesOnDirtyFlag() const { return m_SortDrawablesOnDirtyFlag; }
		bool DrawablesEmpty() const { return m_DrawableGameObjects.Empty(); }
		const TileIndex CalculateTileIndex(const float _x, const float _y) const;
		SpatialHashGrid* GetSpatialHashGrid() { return &m_DrawableGameObjects; }

		void SetRenderCollidables(bool _state);
		bool GetRenderCollidables() const { return m_DebugRenderCollidables; }

	private:

		std::vector<GameObject*> m_GameObjects;

		std::vector<GameObject*> m_CollidableGameObjects;
		SpatialHashGrid m_DrawableGameObjects;

		std::queue<GameObject*> m_GameObjectsToRemove;

		bool m_SortDrawablesOnDirtyFlag = false;
		bool m_DebugRenderCollidables = false;

		GameObject* m_Player;
};