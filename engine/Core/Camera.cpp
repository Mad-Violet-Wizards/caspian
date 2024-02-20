#include "engine/pch.hpp"

#include "Camera.hpp"

#include "engine/Core/Components/C_Tags.hpp"
#include "engine/Core/Components/C_Transform.hpp"

Camera::Camera()
	: m_FreeCamera(false)
	, m_OwningGameObject(nullptr)
	, m_Position(0, 0)
	, m_Size(800, 600)
	, m_Zoom(.5f)
	, m_ZoomSpeedFactor(0.1f)
	, m_ZoomDelta(0.f)
	, m_LevelBounds(0.f, 0.f, 800.f, 600.f)
{
	InitializeEventListeners();
}

void Camera::Update(float _dt)
{
	m_BaseView = ApplicationSingleton::Instance().GetView();

	auto& main_instance = ApplicationSingleton::Instance();

#if defined (_DEBUG)
	sf::Vector2i mouse_delta = sf::Vector2i(0, 0);

	if (CameraDebugController* camera_debug_controller = main_instance.GetDebugControllers().GetCameraController())
	{
		if (camera_debug_controller->IsActive())
			if (camera_debug_controller->ValidateMouseDelta())
				mouse_delta = camera_debug_controller->GetMouseDelta();

	}

	m_Position.x += -mouse_delta.x;
	m_Position.y += -mouse_delta.y;

	// Limit camera to world bounds.
	static float min_x = 0.f;
	static float min_y = 0.f;
	const float max_x = m_LevelBounds.width - m_Size.x;
	const float max_y = m_LevelBounds.height - m_Size.y;

	const sf::Vector2f screen_size{ static_cast<float>(sf::VideoMode::getDesktopMode().width), static_cast<float>(sf::VideoMode::getDesktopMode().height) };

	if (m_Position.x - screen_size.x / 2.f < min_x) m_Position.x = min_x + screen_size.x / 2.f;
	if (m_Position.y - screen_size.y / 2.f < min_y) m_Position.y = min_y + screen_size.y / 2.f;
	if (m_Position.x + screen_size.x / 2.f > max_x) m_Position.x = max_x - screen_size.x / 2.f;
	if (m_Position.y + screen_size.y / 2.f > max_y) m_Position.y = max_y - screen_size.y / 2.f;

	m_BaseView.setCenter(m_Position);
	m_BaseView.setSize(m_Size);
	m_BaseView.setViewport({ 0.f, 0.f, 1.f, 1.f });

	/*if (m_ZoomDelta != 0.f)
	{
		if (m_ZoomDelta < 0.f)
		{
			const float factor = 1.1f;
			m_Zoom += 0.1f;
			m_BaseView.zoom(factor);
		}
		else
		{
			const float factor = 0.9f;
			m_Zoom += -0.1f;
			m_BaseView.zoom(factor);
		}

		m_ZoomDelta = 0.f;
	}*/
#endif
}

void Camera::SetFreeCamera(bool _freecam)
{
	m_FreeCamera = _freecam;
}

bool Camera::IsFreeCamera() const
{
	return m_FreeCamera;
}

void Camera::SetPosition(const sf::Vector2f& _pos)
{
	m_Position = _pos;
}

void Camera::SetViewport(const sf::Vector2f& _viewport)
{
	m_Size = _viewport;
}

void Camera::AddPosition(const sf::Vector2f& _pos)
{
	m_Position += _pos;
}

void Camera::AddViewport(const sf::Vector2f& _viewport)
{
	m_Size += _viewport;
}

const sf::Vector2f& Camera::GetPosition() const
{
	return m_Position;
}

const sf::Vector2f& Camera::GetViewport() const
{
	return m_Size;
}

void Camera::SetZoom(float _zoom)
{
	m_Zoom = _zoom;
}

void Camera::AddZoom(float _zoom)
{
	m_ZoomDelta = _zoom;
}

float Camera::GetZoom() const
{
	return m_Zoom;
}

const sf::View& Camera::GetBaseView() const
{
	return m_BaseView;
}

void Camera::AttachToGameObject(GameObject* game_object)
{
	if (!game_object)
	{
		// Assert Call DetachFromGameObject().
		return;
	}

	if (game_object->HasComponent<C_Tags>())
	{
		if (auto c_tags = game_object->GetComponent<C_Tags>())
		{
			if (c_tags->HasTag(ETag::CameraAttachable))
			{
				m_OwningGameObject = game_object;
			}
		}
	}
}

void Camera::DetachFromGameObject()
{
	if (m_OwningGameObject)
	{
		m_OwningGameObject = nullptr;
	}
}

void Camera::SetLevelBounds(const sf::FloatRect& _bounds)
{
	m_LevelBounds = _bounds;
}

void Camera::InitializeEventListeners()
{
	m_WindowResizeListener = std::make_unique<Events::Listener>();
	m_WindowResizeListener->NotifyOn(sf::Event::Resized);

	m_WindowResizeListener->SetCallback([&](const sf::Event& _event)
		{
			m_Size = sf::Vector2f(_event.size.width, _event.size.height);
		});

	ApplicationSingleton::Instance().GetEngineController().GetEventDispatcher()->AddObserver(m_WindowResizeListener.get());
}
