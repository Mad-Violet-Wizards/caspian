#include "engine/pch.hpp"

#include "EditControllers.hpp"
#include "engine/Tools/imgui-tools-impl/ToolsImpl.hpp"

/////////////////////////////////////////////////////////////
EditControllersManager::EditControllersManager()
{
	m_DebugControllers[EDebugControllerType::Camera] = std::make_unique<CameraEditController>();
	m_DebugControllers[EDebugControllerType::Level] = std::make_unique<LevelEditController>();
}

void EditControllersManager::Update(float _dt)
{
	for (const auto& [controller_type, controller] : m_DebugControllers)
	{
		controller->Update(_dt);
	}
}

CameraEditController* EditControllersManager::GetCameraController() const
{
	return static_cast<CameraEditController*>(m_DebugControllers.at(EDebugControllerType::Camera).get());
}

LevelEditController* EditControllersManager::GetLevelController() const
{
		return static_cast<LevelEditController*>(m_DebugControllers.at(EDebugControllerType::Level).get());
}

/////////////////////////////////////////////////////////////
CameraEditController::CameraEditController()
	: m_LastMousePos(0, 0)
	, m_MouseDelta(0, 0)
{
	InitializeEventListeners();
}

void CameraEditController::Update(float _dt)
{
	if (!m_Active)
		return;
	else
		m_MouseDelta = sf::Vector2i(0, 0);

	auto& main_instance = ApplicationSingleton::Instance();

	sf::Vector2i mouse_pos = main_instance.GetMousePosition();

	if (sf::Mouse::isButtonPressed(sf::Mouse::Middle))
		m_MouseDelta = mouse_pos - m_LastMousePos;

	m_LastMousePos = mouse_pos;
}

bool CameraEditController::ValidateMouseDelta() const
{
	return m_MouseDelta != sf::Vector2i(0, 0);
}

const sf::Vector2i& CameraEditController::GetMouseDelta() const
{
	return m_MouseDelta;
}

void CameraEditController::OnZoomChanged(float _delta)
{

}
 
void CameraEditController::InitializeEventListeners()
{

}

void CameraEditController::OnActivated()
{
}

void CameraEditController::OnDeactivated()
{
}

/////////////////////////////////////////////////////////////
LevelEditController::LevelEditController()
	: m_Mode(ELevelEditControllerMode::None)
	, m_PrevMode(ELevelEditControllerMode::None)
	, m_TilesetUUID(Random::EMPTY_UUID)
	, m_TileSize(0)
	, m_TileX(0)
	, m_TileY(0)
{

}


LevelEditController::~LevelEditController()
{

}

void LevelEditController::Update(float _dt)
{
	if (!m_Active)
		return;

	if (ApplicationSingleton::Instance().GetEngineController().GetToolsManager()->IsImGuiActive())
		return;

	UpdateHighlightTile();

	switch (m_Mode)
	{
		case ELevelEditControllerMode::PaintTile:
		{
			if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
			{
				OnPaint();
			}
			break;
		}
		case ELevelEditControllerMode::Erase:
			if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
			{	
				OnErase();
			}
			break;
		case ELevelEditControllerMode::EditCollisions:
		{
			if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
			{
				OnPlaceNewCollision();
				break;
			}
			else if (sf::Mouse::isButtonPressed(sf::Mouse::Right))
			{
				OnRemoveCollision();
				break;
			}
		}
		case ELevelEditControllerMode::None:
		{
			break;
		}
		default:
			break;
	}
}

void LevelEditController::SetMode(ELevelEditControllerMode _mode)
{
	m_PrevMode = m_Mode;
	m_Mode = _mode;

	OnStateChanged();
}

void LevelEditController::SetWorkingLayer(unsigned int _layer)
{
	m_WorkingLayer = _layer;
}
	
void LevelEditController::OnTilesetTileSelected(Random::UUID _tilesetId, unsigned int _tile_x, unsigned int _tile_y)
{
	if (!m_Active)
	{
		return;
	}

	m_TilesetUUID = _tilesetId;
	m_TileX = _tile_x;
	m_TileY = _tile_y;

	ApplicationSingleton::Instance().GetRenderingSystem()->RefreshHighlightTileSprite(m_TilesetUUID, m_TileX, m_TileY, m_TileSize);
}

void LevelEditController::OnLevelActivated(Level::Level* _level)
{
		m_TileSize = _level->GetTilesSize();
}

void LevelEditController::OnLevelDeactivated()
{
	m_TileSize = 0;
}

void LevelEditController::OnStateChanged()
{
	switch (m_Mode)
	{
		case ELevelEditControllerMode::Erase:
		{
			ApplicationSingleton::Instance().GetRenderingSystem()->ClearHighlightTile();
			break;
		}
		case ELevelEditControllerMode::EditCollisions:
		{
			ApplicationSingleton::Instance().GetRenderingSystem()->ClearHighlightTile();
			ApplicationSingleton::Instance().GetRenderingSystem()->RenderCollisionEdit(true);
			ApplicationSingleton::Instance().GetEngineController().GetToolsManager()->ShowNotification(Tools_Impl::ENotificationType::Info, "Showed collision grid.");
			break;
		}
	}

	switch (m_PrevMode)
	{
		case ELevelEditControllerMode::EditCollisions:
		{
			ApplicationSingleton::Instance().GetEngineController().GetToolsManager()->ShowNotification(Tools_Impl::ENotificationType::Info, "Hidden collision grid.");
			ApplicationSingleton::Instance().GetRenderingSystem()->RenderCollisionEdit(false);
			break;
		}
	}
}

const sf::Vector2u LevelEditController::RoundMouseWorldPosition(const sf::Vector2f& _pos, unsigned int _tileSize) const
{
	if (_tileSize == 0)
		return sf::Vector2u(0, 0);

	const sf::Vector2u rounded_pos
	{
		Math::tile_round_up(static_cast<unsigned int>(_pos.x), _tileSize),
		Math::tile_round_up(static_cast<unsigned int>(_pos.y), _tileSize)
	};

	return rounded_pos;
}

void LevelEditController::OnPaint()
{
	if (m_TileSize == 0 || m_WorkingLayer == -1 || m_TilesetUUID == Random::EMPTY_UUID) return;

	const sf::Vector2u tileset_tile_pos{ m_TileX, m_TileY };
	const sf::Vector2u mouse_world_pos_rounded = RoundMouseWorldPosition(ApplicationSingleton::Instance().GetMousePositionWorld(), m_TileSize);

	ApplicationSingleton::Instance().GetWorld()->PaintTile(mouse_world_pos_rounded, m_TilesetUUID, tileset_tile_pos, m_WorkingLayer);
}

void LevelEditController::OnErase()
{
	if (m_WorkingLayer == -1 || m_TileSize == 0)
		return;

	const sf::Vector2u mouse_world_pos_rounded = RoundMouseWorldPosition(ApplicationSingleton::Instance().GetMousePositionWorld(), m_TileSize);

	ApplicationSingleton::Instance().GetWorld()->EraseTile(mouse_world_pos_rounded, m_WorkingLayer);
}

void LevelEditController::OnPlaceNewCollision()
{
	if (m_TileSize == 0)
		return;

	const sf::Vector2u mouse_world_pos_rounded = RoundMouseWorldPosition(ApplicationSingleton::Instance().GetMousePositionWorld(), m_TileSize);

	ApplicationSingleton::Instance().GetEngineController().GetCollisionsManager()->OnLevelCollisionPlaced(mouse_world_pos_rounded, m_TileSize);
}

void LevelEditController::OnRemoveCollision()
{
	if (m_TileSize == 0)
		return;

	const sf::Vector2u mouse_world_pos_rounded = RoundMouseWorldPosition(ApplicationSingleton::Instance().GetMousePositionWorld(), m_TileSize);

	ApplicationSingleton::Instance().GetEngineController().GetCollisionsManager()->OnLevelCollisionRemoved(mouse_world_pos_rounded, m_TileSize);
}

void LevelEditController::UpdateHighlightTile()
{
	if (m_TileSize == 0)
		return;

	const sf::Vector2u highlight_tile_pos = RoundMouseWorldPosition(ApplicationSingleton::Instance().GetMousePositionWorld(), m_TileSize);

	ApplicationSingleton::Instance().GetRenderingSystem()->RefreshHighlightTilePosition(highlight_tile_pos);

}

/////////////////////////////////////////////////////////////
void IEditorController::SetActive(bool _state)
{
	m_Active = _state;

	m_Active ? OnActivated() : OnDeactivated();
}
