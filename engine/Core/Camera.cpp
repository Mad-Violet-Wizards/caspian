#include "engine/pch.hpp"

#include "Camera.hpp"

#include "engine/Core/Components/C_Tags.hpp"
#include "engine/Core/Components/C_Transform.hpp"

Camera::Camera()
	: m_OwningGameObject(nullptr)
	, m_Position(0, 0)
	, m_LevelBounds(0.f, 0.f, 1920.f, 1080.f)
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

	const sf::Vector2f& render_size = Rendering::System::s_RenderSize;

	// Limit camera to world bounds.
	static float min_x = 0.f;
	static float min_y = 0.f;
	const float max_x = m_LevelBounds.width - render_size.x;
	const float max_y = m_LevelBounds.height - render_size.y;

	if (m_Position.x - (render_size.x / 2.f) < min_x) m_Position.x = min_x + (render_size.x / 2.f);
	if (m_Position.y - (render_size.y / 2.f) < min_y) m_Position.y = min_y + (render_size.y / 2.f);
	if (m_Position.x + (render_size.x / 2.f) > max_x) m_Position.x = max_x - (render_size.x / 2.f);
	if (m_Position.y + (render_size.y / 2.f) > max_y) m_Position.y = max_y - (render_size.y / 2.f);

	m_BaseView.setCenter(m_Position);
	m_BaseView.setSize(render_size);
	m_BaseView.setViewport({ 0.f, 0.f, 1.f, 1.f });
#endif
}

void Camera::SetPosition(const sf::Vector2f& _pos)
{
	m_Position = _pos;
}

void Camera::AddPosition(const sf::Vector2f& _pos)
{
	m_Position += _pos;
}

const sf::Vector2f& Camera::GetPosition() const
{
	return m_Position;
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
}
