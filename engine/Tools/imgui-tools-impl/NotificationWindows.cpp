#include "engine/pch.hpp"

#include <imgui/imgui.h>

#include "ToolsImpl.hpp"
#include "NotificationWindows.hpp"

namespace Tools_Impl
{
	namespace notifications
	{
		WarningWindow::WarningWindow(Manager* _mgr)
			: INotificationWindow(_mgr) {}

		void WarningWindow::Render()
		{
			if (!m_Active)
				return;

			static auto flags = utils::flags();

			float alpha = 1.f - m_DisplayTime / m_MaxDisplayTime;

			ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(1.f, 1.f, 0.f, alpha));
			ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(1.f, 1.f, 0.f, alpha));

			if (ImGui::Begin("Warning Window", &m_Active, flags.message_window_flags))
			{
				ImGui::SetWindowPos(m_Pos);
				ImGui::SetWindowSize(utils::TOOLS_NOTIFICATION_SIZE);
				ImGui::Text(&m_message[0]);
			}

			ImGui::End();
			ImGui::PopStyleColor(2);
		}

		////////////////////////////////////////////////////////////
		ErrorWindow::ErrorWindow(Manager* _mgr) 
			: INotificationWindow(_mgr) {}

		void ErrorWindow::Render()
		{
			if (!m_Active)
				return;

			static auto flags = utils::flags();

			float alpha = 1.f - m_DisplayTime / m_MaxDisplayTime;

			ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(1.f, 0.f, 0.f, alpha));
			ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(1.f, 0.f, 0.f, alpha));

			if (ImGui::Begin("Error Window", &m_Active, flags.message_window_flags))
			{
				ImGui::SetWindowPos(m_Pos);
				ImGui::SetWindowSize(utils::TOOLS_NOTIFICATION_SIZE);

				ImGui::Text(&m_message[0]);
			}

			ImGui::End();
			ImGui::PopStyleColor(2);
		}
	}
}