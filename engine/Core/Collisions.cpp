#include "engine/pch.hpp"
#include "Collisions.hpp"

Collisions::Manager::~Manager()
{
	if (m_CollisionSolverImpl)
	{
		delete m_CollisionSolverImpl;
		m_CollisionSolverImpl = nullptr;
	}
}

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

const Collisions::QuadtreeSolver* Collisions::Manager::GetQuadtreeSolver() const
{
	if (dynamic_cast<QuadtreeSolver*>(m_CollisionSolverImpl))
		return static_cast<QuadtreeSolver*>(m_CollisionSolverImpl);
	else
	{
		std::cout << "Collision solver is not of type QuadtreeSolver\n";
		return nullptr;
	}
}

void Collisions::Manager::ResolveCollisions()
{
	auto it = m_CollidingObjects.begin();

	while (it != m_CollidingObjects.end())
	{
		std::shared_ptr<C_RectCollidable> first = it->first;
		std::shared_ptr<C_RectCollidable> second = it->second;

		IntersectionResult result = first->Intersects(second);

		if (result.m_Collided == false || (first->m_Owner->QueuedForRemoval() || second->m_Owner->QueuedForRemoval()))
		{
			first->m_Owner->OnCollisionExit();
			second->m_Owner->OnCollisionExit();

			it = m_CollidingObjects.erase(it);
		}
		else
		{
			first->m_Owner->OnCollisionStay();
			second->m_Owner->OnCollisionStay();
			++it;
		}
	}
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
				m_CollidingObjects.emplace(colliding_pair);

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

Collisions::QuadtreeSolver::QuadtreeSolver()
{
	m_CurrentHeight = 0;
	m_Bounds = ApplicationSingleton::Instance().GetWorld()->GetActiveLevel()->GetLevelBounds();

	for (unsigned int i = 0; i < (unsigned int)EQuadtreeNode::COUNT; ++i)
		m_Children[i] = nullptr;
}

Collisions::QuadtreeSolver::QuadtreeSolver(unsigned int _height, const sf::FloatRect& bounds)
	: m_CurrentHeight(_height)
	, m_Bounds(bounds)
{
	for (unsigned int i = 0; i < (unsigned int)EQuadtreeNode::COUNT; ++i)
		m_Children[i] = nullptr;
}

/////////////////////////////////////////////////////////////////////////////////////////////////

void Collisions::QuadtreeSolver::Search(const sf::FloatRect& _rect, std::vector<std::shared_ptr<C_RectCollidable>>& _overlapping_objects)
{
	if (m_Children[0] != nullptr)
	{
		EQuadtreeNode _node = CalculateNode(_rect);

		if (_node != EQuadtreeNode::INVALID)
		{
			m_Children[(unsigned int)_node]->Search(_rect, _overlapping_objects);
			return;
		}
	}

	for (auto& collidable : m_Collidables)
	{
		if (_rect.intersects(collidable->GetRect()))
		{
			_overlapping_objects.push_back(collidable);
		}
	}
}

void Collisions::QuadtreeSolver::Insert(std::shared_ptr<C_RectCollidable> _collidable)
{
	if (m_Children[0] != nullptr)
	{
		EQuadtreeNode _node = CalculateNode(_collidable->GetRect());

		if (_node != EQuadtreeNode::INVALID)
		{
			m_Children[(unsigned int)_node]->Insert(_collidable);
			return;
		}
	}

	m_Collidables.emplace_back(_collidable);

	if (m_Collidables.size() > QuadtreeSolver::MAX_OBJECTS && m_CurrentHeight <= QuadtreeSolver::MAX_HEIGHT)
	{	
		if (m_Children[0] == nullptr)
		{
			const unsigned int half_width = m_Bounds.width / 2;
			const unsigned int half_height = m_Bounds.height / 2;

			const sf::FloatRect top_left(m_Bounds.left, m_Bounds.top, half_width, half_height);
			m_Children[0] = std::make_shared<QuadtreeSolver>(m_CurrentHeight + 1, top_left);

			const sf::FloatRect top_right(m_Bounds.left + half_width, m_Bounds.top, half_width, half_height);
			m_Children[1] = std::make_shared<QuadtreeSolver>(m_CurrentHeight + 1, top_right);

			const sf::FloatRect bot_left(m_Bounds.left + half_width, m_Bounds.top + half_height, half_width, half_height);
			m_Children[2] = std::make_shared<QuadtreeSolver>(m_CurrentHeight + 1, bot_left);

			const sf::FloatRect bot_right(m_Bounds.left + half_width, m_Bounds.top + half_height, half_width, half_height);
			m_Children[3] = std::make_shared<QuadtreeSolver>(m_CurrentHeight + 1, bot_right);

			for (std::shared_ptr<C_RectCollidable> collidable : m_Collidables)
			{
				EQuadtreeNode _node = CalculateNode(collidable->GetRect());

				if (_node != EQuadtreeNode::INVALID)
				{
					m_Children[(unsigned int)_node]->Insert(collidable);
				}
			}

			m_Collidables.clear();
		}
	}
}

void Collisions::QuadtreeSolver::Clear()
{
		m_Collidables.clear();

		for (unsigned int i = 0; i < (unsigned int)EQuadtreeNode::COUNT; ++i)
		{
			if (m_Children[i] != nullptr)
			{
				m_Children[i]->Clear();
				m_Children[i] = nullptr;
			}
		}
}

const std::vector<std::shared_ptr<C_RectCollidable>>& Collisions::QuadtreeSolver::GetCollidables() const
{
	return m_Collidables;
}

void Collisions::QuadtreeSolver::DrawBounds(sf::RenderWindow& _window) const
{
	if (m_Children[0] != nullptr)
	{
		for (unsigned int i = 0; i < (unsigned int)EQuadtreeNode::COUNT; ++i)
			m_Children[i]->DrawBounds(_window);
	}

	sf::RectangleShape rect(sf::Vector2f(m_Bounds.width, m_Bounds.height));
	rect.setPosition(m_Bounds.left, m_Bounds.top);
	rect.setFillColor(sf::Color::Transparent);
	rect.setOutlineColor(sf::Color::Cyan);
	_window.draw(rect);
}

Collisions::EQuadtreeNode Collisions::QuadtreeSolver::CalculateNode(const sf::FloatRect& _rect)
{
	EQuadtreeNode index = EQuadtreeNode::INVALID;

	const float vertical_midpoint = m_Bounds.left + m_Bounds.width * 0.5f;
	const float horizontal_midpoint = m_Bounds.top + m_Bounds.height * 0.5f;

	const bool top_quadrant = _rect.top - _rect.height >= horizontal_midpoint;
	const bool bottom_quadrant = _rect.top < horizontal_midpoint;
	const bool left_quadrant = _rect.left + _rect.width <= vertical_midpoint;
	const bool right_quadrant = _rect.left > vertical_midpoint;

	if (top_quadrant)
	{
		if (left_quadrant)			 index = EQuadtreeNode::TopLeft;
		else if (right_quadrant) index = EQuadtreeNode::TopRight;
	}
	else if (bottom_quadrant)
	{
		if (left_quadrant)			 index = EQuadtreeNode::BottomLeft;
		else if (right_quadrant) index = EQuadtreeNode::BottomRight;
	}

	return index;

}
