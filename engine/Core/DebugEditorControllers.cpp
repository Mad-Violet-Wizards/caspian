#include "engine/pch.hpp"

#include "DebugEditorControllers.hpp"

/////////////////////////////////////////////////////////////
DebugEditorControllers::DebugEditorControllers()
{
	m_DebugControllers[EDebugControllerType::Camera] = std::make_unique<CameraDebugController>();
	m_DebugControllers[EDebugControllerType::Level] = std::make_unique<LevelDebugController>();
}

void DebugEditorControllers::Update(float _dt)
{
	for (const auto& [controller_type, controller] : m_DebugControllers)
	{
		controller->Update(_dt);
	}
}

CameraDebugController* DebugEditorControllers::GetCameraController() const
{
	return static_cast<CameraDebugController*>(m_DebugControllers.at(EDebugControllerType::Camera).get());
}

LevelDebugController* DebugEditorControllers::GetLevelController() const
{
		return static_cast<LevelDebugController*>(m_DebugControllers.at(EDebugControllerType::Level).get());
}

/////////////////////////////////////////////////////////////
CameraDebugController::CameraDebugController()
	: m_LastMousePos(0, 0)
	, m_MouseDelta(0, 0)
{
	InitializeEventListeners();
}

void CameraDebugController::Update(float _dt)
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

bool CameraDebugController::ValidateMouseDelta() const
{
	return m_MouseDelta != sf::Vector2i(0, 0);
}

const sf::Vector2i& CameraDebugController::GetMouseDelta() const
{
	return m_MouseDelta;
}

void CameraDebugController::OnZoomChanged(float _delta)
{

}
 
void CameraDebugController::InitializeEventListeners()
{

}

void CameraDebugController::OnActivated()
{
}

void CameraDebugController::OnDeactivated()
{
}

/////////////////////////////////////////////////////////////
LevelDebugController::LevelDebugController()
	: m_Mode(ELevelDebugControllerMode::None)
	, m_TilesetUUID(Random::EMPTY_UUID)
	, m_TileSize(0)
	, m_TileX(0)
	, m_TileY(0)
{

}


LevelDebugController::~LevelDebugController()
{

}

void LevelDebugController::Update(float _dt)
{
	if (!m_Active)
		return;

	if (ApplicationSingleton::Instance().GetEngineController().GetToolsManager()->IsImGuiActive())
		return;

	UpdateHighlightTile();

	switch (m_Mode)
	{
		case ELevelDebugControllerMode::Paint:
		{
			if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
			{
				OnPaint();
			}
			break;
		}
		case ELevelDebugControllerMode::Erase:
			if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
			{	
				OnErase();
			}
			break;
		case ELevelDebugControllerMode::None:
		{
			break;
		}
		default:
			break;
	}
}

void LevelDebugController::SetMode(ELevelDebugControllerMode _mode)
{
	if (_mode == ELevelDebugControllerMode::Erase)
	{
		ApplicationSingleton::Instance().GetRenderingSystem()->ClearHighlightTile();
	}

	m_Mode = _mode;
}

void LevelDebugController::SetWorkingLayer(unsigned int _layer)
{
	m_WorkingLayer = _layer;
}
	
void LevelDebugController::OnTilesetTileSelected(Random::UUID _tilesetId, unsigned int _tile_x, unsigned int _tile_y)
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

void LevelDebugController::OnLevelActivated(Level::Level* _level)
{
		m_TileSize = _level->GetTilesSize();
}

const sf::Vector2u LevelDebugController::RoundMouseWorldPosition(const sf::Vector2f& _pos, unsigned int _tileSize) const
{
	if (_tileSize == 0)
		return sf::Vector2u(0, 0);

	const sf::Vector2u rounded_pos
	{
		Math::round_up(static_cast<unsigned int>(_pos.x), _tileSize) - _tileSize,
		Math::round_up(static_cast<unsigned int>(_pos.y), _tileSize) - _tileSize
	};

	return rounded_pos;
}

void LevelDebugController::OnPaint()
{
	const sf::Vector2u tileset_tile_pos{ m_TileX, m_TileY };
	const sf::Vector2u mouse_world_pos_rounded = RoundMouseWorldPosition(ApplicationSingleton::Instance().GetMousePositionWorld(), m_TileSize);

	if (m_TileSize == 0 || m_TilesetUUID == Random::EMPTY_UUID) return;

	ApplicationSingleton::Instance().GetWorld()->PaintTile(mouse_world_pos_rounded, m_TilesetUUID, tileset_tile_pos, m_WorkingLayer);
}

void LevelDebugController::OnErase()
{
	const sf::Vector2u mouse_world_pos_rounded = RoundMouseWorldPosition(ApplicationSingleton::Instance().GetMousePositionWorld(), m_TileSize);

	if (m_WorkingLayer == -1)
		return;

	ApplicationSingleton::Instance().GetWorld()->EraseTile(mouse_world_pos_rounded, m_WorkingLayer);
}

void LevelDebugController::UpdateHighlightTile()
{
	if (m_TileSize == 0)
		return;

	const sf::Vector2u highlight_tile_pos = RoundMouseWorldPosition(ApplicationSingleton::Instance().GetMousePositionWorld(), m_TileSize);

	ApplicationSingleton::Instance().GetRenderingSystem()->RefreshHighlightTilePosition(highlight_tile_pos);

}

/////////////////////////////////////////////////////////////
void IDebugController::SetActive(bool _state)
{
	m_Active = _state;

	m_Active ? OnActivated() : OnDeactivated();
}
