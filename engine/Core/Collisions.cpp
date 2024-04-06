#include "engine/pch.hpp"
#include "Collisions.hpp"

void Collisions::Manager::SetCollisionSolver(ECollisionSolver _collision_solver)
{
	m_CollisionSolver = _collision_solver;
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

		for (const Serializable::Binary::CollisionRectInfo& collision_rect_info : _collision_layer_info->m_CollisionRects)
			ApplicationSingleton::Instance().GetEngineController().GetGameObjectStorage()->ConstructNew(collision_rect_info);

	}
	else
		std::cout << "Collision info already exists for level: " << _level_name << "\n";
}

void Collisions::Manager::OnLevelCollisionPlaced(const sf::Vector2u& _pos, unsigned int m_TileSize)
{
	CachedInfo* cached_info = FindCachedInfo(ApplicationSingleton::Instance().GetWorld()->GetActiveLevel()->GetLevelName());

	Serializable::Binary::CollisionRectInfo new_collision_rect_info;
	new_collision_rect_info.m_Rect.left = _pos.x;
	new_collision_rect_info.m_Rect.top = _pos.y;
	new_collision_rect_info.m_Rect.width = m_TileSize;
	new_collision_rect_info.m_Rect.height = m_TileSize;

	if (cached_info)
	{
		for (const Serializable::Binary::CollisionRectInfo& collision_rect_info : cached_info->m_CollisionLayerInfo->m_CollisionRects)
			if (collision_rect_info.m_Rect.left == _pos.x && collision_rect_info.m_Rect.top == _pos.y && collision_rect_info.m_Rect.width == m_TileSize && collision_rect_info.m_Rect.height == m_TileSize)
				return;

		cached_info->m_CollisionLayerInfo->m_CollisionRects.push_back(new_collision_rect_info);
		cached_info->PerformSave();
	}


	ApplicationSingleton::Instance().GetEngineController().GetGameObjectStorage()->ConstructNew(new_collision_rect_info);
}

void Collisions::Manager::OnLevelCollisionRemoved(const sf::Vector2u& _pos, unsigned int m_TileSize)
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
