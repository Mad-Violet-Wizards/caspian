#include "engine/pch.hpp"

#include "Camera.hpp"

#include "engine/Core/Components/C_Tags.hpp"

Camera::Camera()
	: m_FreeCamera(false)
	, m_Position(0, 0)
	, m_Viewport(1, 1)
{
}

void Camera::Update(float _dt)
{

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
	m_Viewport = _viewport;
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
