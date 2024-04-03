#pragma once

class GameObject;

class SpatialHashGridBucket
{
	public:

		SpatialHashGridBucket();
		~SpatialHashGridBucket();

		void Add(GameObject* _game_object);
		void Remove(GameObject* _game_object);
		const std::vector<GameObject*>& GetConstRef() const { return m_GameObjects; }
		std::vector<GameObject*>& GetRef() { return m_GameObjects; }

		// find
		std::vector<GameObject*>::iterator find(GameObject* _game_object);
		std::vector<GameObject*>::iterator erase(std::vector<GameObject*>::iterator _it);

		// it begin and end
		std::vector<GameObject*>::iterator begin() { return m_GameObjects.begin(); }
		std::vector<GameObject*>::iterator end() { return m_GameObjects.end(); }

		// const begin and end
		std::vector<GameObject*>::const_iterator begin() const { return m_GameObjects.begin(); }
		std::vector<GameObject*>::const_iterator end() const { return m_GameObjects.end(); }

		// const iterator cbegin and cend
		std::vector<GameObject*>::const_iterator cbegin() const { return m_GameObjects.cbegin(); }
		std::vector<GameObject*>::const_iterator cend() const { return m_GameObjects.cend(); }


	private:

		void Sort();

	private:

		std::vector<GameObject*> m_GameObjects;
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

	const std::vector<GameObject*>& GetConstRefToBucket(const TileIndex& _bucket_index) const;
	std::vector<GameObject*>& GetRefToBucket(const TileIndex& _bucket_index);


	std::vector<GameObject*>::iterator begin(const TileIndex& _bucket_index);
	std::vector<GameObject*>::iterator end(const TileIndex& _bucket_index);

	std::vector<GameObject*>::iterator find(GameObject* _game_object);
	std::vector<GameObject*>::iterator end(GameObject* _game_object);
	void Erase(std::vector<GameObject*>::iterator _it);

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
		//void ConstructNew(Serializable::Binary::CollisionRectInfo* _collision_rect_info);
		//void ConstructNew(Serializable::Binary::TextureTileInfo* _texture_tile_info, unsigned int _active_level_tiles_size);

		void ConstructNew(const Serializable::Binary::CollisionRectInfo& _collision_rect_info);
		void ConstructNew(const Serializable::Binary::TextureTileInfo& _texture_tile_info, unsigned int _active_level_tiles_size, unsigned int _tile_layer);

		const std::vector<GameObject*>& GetCollidableGameObjects() const { return m_CollidableGameObjects; }

		void SortDrawablesOnDirtyFlag(bool _state) { m_SortDrawablesOnDirtyFlag = _state; }
		bool GetSortDrawablesOnDirtyFlag() const { return m_SortDrawablesOnDirtyFlag; }
		bool DrawablesEmpty() const { return m_DrawableGameObjects.Empty(); }
		const TileIndex CalculateTileIndex(const float _x, const float _y) const;

		SpatialHashGrid& GetSpatialHashGrid() { return m_DrawableGameObjects; }

	private:

		std::vector<GameObject*> m_GameObjects;

		std::vector<GameObject*> m_CollidableGameObjects;
		SpatialHashGrid m_DrawableGameObjects;

		std::queue<GameObject*> m_GameObjectsToRemove;

		bool m_SortDrawablesOnDirtyFlag = false;

		GameObject* m_Player;
};