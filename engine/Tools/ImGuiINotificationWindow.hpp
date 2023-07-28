#pragma once

#include "ImGuiUtils.hpp"

// TODO: Later move to IImGuiMessageWindow.cpp and include engine/pch.hpp.
#include <string>
#include <string_view>

namespace Tools
{
	class ImGuiINotificationWindow
	{
	public:

		ImGuiINotificationWindow(ImGuiManager* _mgr)
			: m_Manager(_mgr) {}
		virtual ~ImGuiINotificationWindow() = default;

		ImVec2 CalculatePosition(int _active_notifications) const;

		void Update(float _dt);
		virtual void Render() = 0;

		void SetMessage(std::string_view _msg)  {  m_message = _msg; }

		void Show([[maybe_unused]] int _active_notifications);
		void Hide();

	public:

		float m_MaxDisplayTime = utils::TOOLS_NOTIFICATION_MAX_DISPLAY_TIME;
		float m_DisplayTime = 0.f;

		bool m_Active = utils::TOOLS_INIT_ACTIVE_STATE;

		ImVec2 m_Pos{ utils::TOOLS_NOTIFICATION_START_POSITION };

	protected:

		std::string m_message;

		ImGuiManager* m_Manager = nullptr;

	};
};
