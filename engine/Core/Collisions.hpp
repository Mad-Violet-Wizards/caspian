#pragma once

namespace Collisions
{
	struct CachedInfo
	{
		std::string m_LevelName;
		std::shared_ptr<Serializable::Binary::CollisionLayerInfo> m_CollisionLayerInfo;

		void PerformSave();
	};

	enum class ECollisionSolver
	{
		NoCollision,
		BruteForce,
		Quadtree
	};

	class Manager
	{
		public:

			Manager() = default;
			~Manager() = default;

			void SetCollisionSolver(ECollisionSolver _collision_solver);

			CachedInfo* FindCachedInfo(const std::string& _level_name);
			void AddCachedInfo(const std::string& _level_name, std::shared_ptr<Serializable::Binary::CollisionLayerInfo>& _collision_layer_info);

			void OnLevelCollisionPlaced(const sf::Vector2u& _pos, unsigned int m_TileSize);
			void OnLevelCollisionRemoved(const sf::Vector2u& _pos, unsigned int m_TileSize);

		private:

			std::vector<CachedInfo> m_CachedCollisionsInfo;

			ECollisionSolver m_CollisionSolver;
	};
}