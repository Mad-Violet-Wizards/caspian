#include "engine/pch.hpp"
#include "Collisions.hpp"

void Collisions::Manager::Update(float _dt)
{
	if (!m_ProccessingCollisions)
		return;

	ResolveCollisions();

	m_CollisionSolverImpl->Clear();

	GameObjectCollection* game_objects_ptr = ApplicationSingleton::Instance().GetEngineController().GetGameObjectStorage();

	for (auto it = game_objects_ptr->collidable_objects_cbegin(); it != game_objects_ptr->collidable_objects_cend(); ++it)
	{
		m_CollisionSolverImpl->Insert((*it)->GetComponent<C_RectCollidable>());
	}

	FindCollisions();
}

void Collisions::Manager::SetCollisionSolver(ECollisionSolver _collision_solver)
{
	m_CollisionSolverType = _collision_solver;
}

Collisions::CachedInfo* Collisions::Manager::FindCachedInfo(const std::string& _level_name)
{
	for (auto& cached_info : m_CachedCollisionsInfo)
	{
		if (cached_info.m_LevelName == _level_name)
		{
			return &cached_info;
		}
	}

	return nullptr;
}

void Collisions::Manager::AddCachedInfo(const std::string& _level_name, std::shared_ptr<Serializable::Binary::CollisionLayerInfo>& _collision_layer_info)
{
	CachedInfo new_collision_info;
	new_collision_info.m_LevelName = _level_name;
	new_collision_info.m_CollisionLayerInfo = _collision_layer_info;

	if (!FindCachedInfo(_level_name))
	{
		m_CachedCollisionsInfo.push_back(new_collision_info);
	}
	else
		std::cout << "Collision info already exists for level: " << _level_name << "\n";
}

void Collisions::Manager::OnLevelCollisionPlaced(const sf::Vector2u& _pos, unsigned int _tile_size)
{
	CachedInfo* cached_info = FindCachedInfo(ApplicationSingleton::Instance().GetWorld()->GetActiveLevel()->GetLevelName());

	Serializable::Binary::CollisionRectInfo new_collision_rect_info;
	new_collision_rect_info.m_Rect.left = _pos.x;
	new_collision_rect_info.m_Rect.top = _pos.y;
	new_collision_rect_info.m_Rect.width = _tile_size;
	new_collision_rect_info.m_Rect.height = _tile_size;

	if (cached_info)
	{
		for (const Serializable::Binary::CollisionRectInfo& collision_rect_info : cached_info->m_CollisionLayerInfo->m_CollisionRects)
			if (collision_rect_info.m_Rect.left == _pos.x && collision_rect_info.m_Rect.top == _pos.y && collision_rect_info.m_Rect.width == _tile_size && collision_rect_info.m_Rect.height == _tile_size)
				return;

		cached_info->m_CollisionLayerInfo->m_CollisionRects.push_back(new_collision_rect_info);
		cached_info->PerformSave();
	}


	ApplicationSingleton::Instance().GetEngineController().GetGameObjectStorage()->ConstructNew(new_collision_rect_info);
}

void Collisions::Manager::OnLevelCollisionRemoved(const sf::Vector2u& _pos, unsigned int _tile_size)
{
	auto& game_objects_collidable_vec = ApplicationSingleton::Instance().GetEngineController().GetGameObjectStorage()->GetCollidableGameObjects();

	CachedInfo* cached_info = FindCachedInfo(ApplicationSingleton::Instance().GetWorld()->GetActiveLevel()->GetLevelName());

	if (cached_info)
	{
		for (auto it = cached_info->m_CollisionLayerInfo->m_CollisionRects.begin(); it != cached_info->m_CollisionLayerInfo->m_CollisionRects.end(); ++it)
		{
			if (it->m_Rect.left == _pos.x && it->m_Rect.top == _pos.y)
			{
				cached_info->m_CollisionLayerInfo->m_CollisionRects.erase(it);
				break;
			}
		}

		cached_info->PerformSave();
	}


	for (GameObject* collidable_go : game_objects_collidable_vec)
	{
		if (auto transform_component_sPtr = collidable_go->GetComponent<C_Transform>())
		{
			const sf::Vector2u go_pos = (sf::Vector2u)transform_component_sPtr->GetPosition();

			if (go_pos == _pos)
			{
				collidable_go->QueueForRemoval();
				break;
			}
		}
	}
}

void Collisions::Manager::OnLevelActivated(const Levels::Level* _activated_lvl)
{
	CachedInfo* cached_info = FindCachedInfo(_activated_lvl->GetLevelName());

	if (!cached_info)
	{
		std::cout << "No cached collision info found for level: " << _activated_lvl->GetLevelName() << "\n";
		return;
	}
	
	for (const Serializable::Binary::CollisionRectInfo& collision_rect_info : cached_info->m_CollisionLayerInfo->m_CollisionRects)
	{
		ApplicationSingleton::Instance().GetEngineController().GetGameObjectStorage()->ConstructNew(collision_rect_info);
	}
}

void Collisions::Manager::StartProcessingCollisions()
{
	m_ProccessingCollisions = true;

	if (m_CollisionSolverImpl == nullptr)
	{
		switch (m_CollisionSolverType)
		{
			case ECollisionSolver::NoCollision:
				m_CollisionSolverImpl = new NoCollisionSolver();
				break;
			case ECollisionSolver::BruteForce:
				m_CollisionSolverImpl = new BruteForceSolver();
				break;
			case ECollisionSolver::Quadtree:
				m_CollisionSolverImpl = new QuadtreeSolver();
				break;
			default:
				std::cout << "Invalid collision solver type\nSetting to NoCollisionSolver.\n";
				m_CollisionSolverImpl = new NoCollisionSolver();
				break;
		}
	}
}

void Collisions::Manager::StopProcessingCollisions()
{
	delete m_CollisionSolverImpl;
	m_CollisionSolverImpl = nullptr;

	m_ProccessingCollisions = false;
}

void Collisions::Manager::ResolveCollisions()
{
	// CONTINUE HERE.
}

void Collisions::Manager::FindCollisions()
{
	GameObjectCollection* game_objects_ptr = ApplicationSingleton::Instance().GetEngineController().GetGameObjectStorage();

	for (auto it = game_objects_ptr->collidable_objects_cbegin(); it != game_objects_ptr->collidable_objects_cend(); ++it)
	{
		auto transform_component_sPtr = (*it)->GetComponent<C_Transform>();
		auto collision_rect_component_sPtr = (*it)->GetComponent<C_RectCollidable>();

		if (transform_component_sPtr->IsStatic())
			continue;

		std::vector<std::shared_ptr<C_RectCollidable>> colliding_objects;
		m_CollisionSolverImpl->Search(collision_rect_component_sPtr->GetRect(), colliding_objects);

		for (const std::shared_ptr<C_RectCollidable>& colliding_object : colliding_objects)
		{
			if (colliding_object->m_Owner == (*it))
				continue;

			IntersectionResult result = collision_rect_component_sPtr->Intersects(colliding_object);

			if (result.m_Collided)
			{
				ObjectsCollidingPair colliding_pair(collision_rect_component_sPtr, colliding_object);
				m_CollidingObjects.emplace_back(colliding_pair);

				collision_rect_component_sPtr->m_Owner->OnCollisionEnter();
				colliding_object->m_Owner->OnCollisionEnter();

				collision_rect_component_sPtr->FixDistance(result);
			}
		}
	}
}

void Collisions::CachedInfo::PerformSave()
{
	if (!ApplicationSingleton::Instance().GetWorld()->IsLevelActive())
		return;

	const std::string& active_level_name = ApplicationSingleton::Instance().GetWorld()->GetActiveLevel()->GetLevelName() + fs::IFile::TypeToStringExt(fs::IFile::EType::Data_Collisions);

	std::filesystem::path save_path = "levels";
	save_path /= active_level_name;

	fs::IFileSystem* data_fs = ApplicationSingleton::Instance().GetEngineController().GetFilesystemManager()->Get("data");

	if (std::shared_ptr<fs::IFile> collision_file = data_fs->OpenFile(save_path.string(), fs::io::OpenMode::Out | fs::io::OpenMode::Binary))
	{
		collision_file->Seek(0, fs::io::Origin::Begin);
		collision_file->SerializeBinary(m_CollisionLayerInfo);
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////

void Collisions::NoCollisionSolver::Search(const sf::FloatRect& _rect, std::vector<std::shared_ptr<C_RectCollidable>>& _overlapping_objects)
{

}

void Collisions::NoCollisionSolver::Insert(std::shared_ptr<C_RectCollidable> _collidable)
{
	
}

void Collisions::NoCollisionSolver::Clear()
{

}

/////////////////////////////////////////////////////////////////////////////////////////////////

void Collisions::BruteForceSolver::Search(const sf::FloatRect& _rect, std::vector<std::shared_ptr<C_RectCollidable>>& _overlapping_objects)
{
	for (std::shared_ptr<C_RectCollidable> collidable : m_Collidables)
	{
		if (_rect.intersects(collidable->GetRect()))
			_overlapping_objects.push_back(collidable);
	}
}

void Collisions::BruteForceSolver::Insert(std::shared_ptr<C_RectCollidable> _collidable)
{
	m_Collidables.push_back(_collidable);
}

void Collisions::BruteForceSolver::Clear()
{
	m_Collidables.clear();
}

/////////////////////////////////////////////////////////////////////////////////////////////////

void Collisions::QuadtreeSolver::Search(const sf::FloatRect& _rect, std::vector<std::shared_ptr<C_RectCollidable>>& _overlapping_objects)
{
}

void Collisions::QuadtreeSolver::Insert(std::shared_ptr<C_RectCollidable> _collidable)
{

}

void Collisions::QuadtreeSolver::Clear()
{

}
