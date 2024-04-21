#pragma once
#include "Level.hpp"
#include "engine/Core/Components/C_RectCollidable.hpp"

namespace Collisions
{
	using namespace Levels;
	using ObjectsCollidingPair = std::pair<std::shared_ptr<C_RectCollidable>, std::shared_ptr<C_RectCollidable>>;

	class ICollisionSolver
	{
	public:

		ICollisionSolver() = default;
		virtual ~ICollisionSolver() = default;

		virtual void Search(const sf::FloatRect& _rect, std::vector<std::shared_ptr<C_RectCollidable>>& _overlapping_objects) = 0;
		virtual void Insert(std::shared_ptr<C_RectCollidable> _collidable) = 0;
		virtual void Clear() = 0;
	};

	class NoCollisionSolver : public ICollisionSolver
	{
	public:

		NoCollisionSolver() = default;
		~NoCollisionSolver() = default;

		void Search(const sf::FloatRect& _rect, std::vector<std::shared_ptr<C_RectCollidable>>& _overlapping_objects) override;
		void Insert(std::shared_ptr<C_RectCollidable> _collidable) override;
		void Clear() override;
	};

	class BruteForceSolver : public ICollisionSolver
	{
	public:

		BruteForceSolver() = default;
		~BruteForceSolver() = default;

		void Search(const sf::FloatRect& _rect, std::vector<std::shared_ptr<C_RectCollidable>>& _overlapping_objects) override;
		void Insert(std::shared_ptr<C_RectCollidable> _collidable) override;
		void Clear() override;

	private:

		std::vector<std::shared_ptr<C_RectCollidable>> m_Collidables;
	};

	enum class EQuadtreeNode : unsigned int
	{
		TopLeft = 0,
		TopRight,
		BottomLeft,
		BottomRight,
		COUNT = 4,
		INVALID = 5
	};

	class QuadtreeSolver : public ICollisionSolver
	{
	public:

		QuadtreeSolver();
		QuadtreeSolver(unsigned int _height, const sf::FloatRect& bounds);
		~QuadtreeSolver() = default;

		void Search(const sf::FloatRect& _rect, std::vector<std::shared_ptr<C_RectCollidable>>& _overlapping_objects) override;
		void Insert(std::shared_ptr<C_RectCollidable> _collidable) override;
		void Clear() override;

#ifdef DEBUG
		void DrawBounds(sf::RenderWindow& _window) const;
#endif
	
	private:
		
		EQuadtreeNode CalculateNode(const sf::FloatRect& _rect);

	private:

		static const unsigned int MAX_HEIGHT = 8;
		static const unsigned int MAX_OBJECTS = 8;

	private:

		std::array<std::shared_ptr<QuadtreeSolver>, 4> m_Children;

		std::vector<std::shared_ptr<C_RectCollidable>> m_Collidables;

		sf::FloatRect m_Bounds;
		unsigned int m_CurrentHeight;
	};

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
			~Manager();

			void Update(float _dt);
			void SetCollisionSolver(ECollisionSolver _collision_solver);

			CachedInfo* FindCachedInfo(const std::string& _level_name);
			void AddCachedInfo(const std::string& _level_name, std::shared_ptr<Serializable::Binary::CollisionLayerInfo>& _collision_layer_info);

			void OnLevelCollisionPlaced(const sf::Vector2u& _pos, unsigned int _tile_size);
			void OnLevelCollisionRemoved(const sf::Vector2u& _pos, unsigned int _tile_size);
			void OnLevelActivated(const Levels::Level* _activated_lvl);

			void StartProcessingCollisions();
			void StopProcessingCollisions();

#ifdef DEBUG
			const QuadtreeSolver* GetQuadtreeSolver() const;
#endif

		private:

			void ResolveCollisions();
			void FindCollisions();

		private:

			bool m_ProccessingCollisions = false;

			std::vector<CachedInfo> m_CachedCollisionsInfo;
			std::set<ObjectsCollidingPair> m_CollidingObjects;


			ECollisionSolver m_CollisionSolverType = ECollisionSolver::NoCollision;
			ICollisionSolver* m_CollisionSolverImpl = nullptr;

	};
}