#include "engine/pch.hpp"

#include "DebugEditorControllers.hpp"

/////////////////////////////////////////////////////////////
DebugEditorControllers::DebugEditorControllers()
{
	m_DebugControllers[EDebugControllerType::Camera] = std::make_unique<CameraDebugController>();
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
void IDebugController::SetActive(bool _state)
{
	m_Active = _state;

	m_Active ? OnActivated() : OnDeactivated();
}
