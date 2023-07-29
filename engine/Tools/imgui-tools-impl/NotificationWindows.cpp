#include "engine/pch.hpp"

#include <imgui/imgui.h>

#include "ToolsImpl.hpp"
#include "NotificationWindows.hpp"

namespace Tools_Impl
{
	namespace notifications
	{
		////////////////////////////////////////////////////////////
		/* WarningNotificationWindow */
		////////////////////////////////////////////////////////////
		WarningNotificationWindow::WarningNotificationWindow(Manager* _mgr)
			: INotificationWindow(_mgr) {}

		void WarningNotificationWindow::Render()
		{
			if (!m_Active)
				return;

			static auto flags = utils::flags();

			float alpha = 1.f - m_DisplayTime / m_MaxDisplayTime;

			if (alpha < utils::TOOLS_NOTIFICATION_MIN_ALPHA)
				alpha = utils::TOOLS_NOTIFICATION_MIN_ALPHA;

			ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.15f, 0.15f, 0.15f, alpha));
			ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.78f, 0.57f, 0.15f, alpha));

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
		/* ErrorNotificationWindow */
		////////////////////////////////////////////////////////////
		ErrorNotificationWindow::ErrorNotificationWindow(Manager* _mgr)
			: INotificationWindow(_mgr) {}

		void ErrorNotificationWindow::Render()
		{
			if (!m_Active)
				return;

			static auto flags = utils::flags();

			float alpha = 1.f - m_DisplayTime / m_MaxDisplayTime;

			if (alpha < utils::TOOLS_NOTIFICATION_MIN_ALPHA)
				alpha = utils::TOOLS_NOTIFICATION_MIN_ALPHA;

			ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.15f, 0.15f, 0.15f, alpha));
			ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(1.f, 0.14f, 0.1f, alpha));

			if (ImGui::Begin("Error Window", &m_Active, flags.message_window_flags))
			{
				ImGui::SetWindowPos(m_Pos);
				ImGui::SetWindowSize(utils::TOOLS_NOTIFICATION_SIZE);

				ImGui::Text(&m_message[0]);
			}

			ImGui::End();
			ImGui::PopStyleColor(2);
		}

		////////////////////////////////////////////////////////////
		/* SucessNotificationWindow */
		////////////////////////////////////////////////////////////
		SuccessNotificationWindow::SuccessNotificationWindow(Manager* _mgr)
			: INotificationWindow(_mgr) {}

		void SuccessNotificationWindow::Render()
		{
			if (!m_Active)
				return;

			static auto flags = utils::flags();

			float alpha = 1.f - m_DisplayTime / m_MaxDisplayTime;

			if (alpha < utils::TOOLS_NOTIFICATION_MIN_ALPHA)
				alpha = utils::TOOLS_NOTIFICATION_MIN_ALPHA;

			ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.15f, 0.15f, 0.15f, alpha));
			ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.07f, 0.8f, 0.17f, alpha));

			if (ImGui::Begin("Success Window", &m_Active, flags.message_window_flags))
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