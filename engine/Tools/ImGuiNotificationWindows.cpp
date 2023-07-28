#include "engine/pch.hpp"

#include <imgui/imgui.h>

#include "ImGuiTools.hpp"
#include "ImGuiNotificationWindows.hpp"

namespace Tools
{
	namespace notifications
	{
		WarningWindow::WarningWindow(ImGuiManager* _mgr)
			: ImGuiINotificationWindow(_mgr) {}

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
		ErrorWindow::ErrorWindow(ImGuiManager* _mgr) 
			: ImGuiINotificationWindow(_mgr) {}

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