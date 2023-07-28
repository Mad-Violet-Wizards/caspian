#include "engine/pch.hpp"

#include "ImGuiTools.hpp"

#include "ImGuiINotificationWindow.hpp"

using namespace Tools;

ImVec2 ImGuiINotificationWindow::CalculatePosition(int _active_notifications) const
{
	ImVec2 pos{ utils::TOOLS_NOTIFICATION_START_POSITION };

	if (_active_notifications < 1)
		return pos;

	ImVec2 size{ utils::TOOLS_NOTIFICATION_SIZE };

	constexpr float margin_top = 10;

	pos.y += _active_notifications * size.y + margin_top;

	return pos;
}

void ImGuiINotificationWindow::Update(float _dt)
{
	if (m_Active)
		m_DisplayTime += _dt;

	if (m_DisplayTime > m_MaxDisplayTime)
		Hide();
}

void ImGuiINotificationWindow::Show(int _active_notifications)
{
	if (m_Active)
	{
		m_DisplayTime = 0.f;
		return;
	}

	if (_active_notifications >= 1)
		m_Pos = CalculatePosition(_active_notifications);

	m_Active = true;
}

void ImGuiINotificationWindow::Hide()
{
	m_DisplayTime = 0.f;
	m_Active = false;
	m_Pos = utils::TOOLS_NOTIFICATION_START_POSITION;

	m_Manager->m_NotificationManager.OnNotificationHidden();
}
