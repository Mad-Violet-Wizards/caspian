#pragma once

#include <imgui/imgui.h>

/////////////////////////////////////////////////////////
namespace Tools_Impl
{
	namespace utils
	{
		constexpr bool TOOLS_INIT_ACTIVE_STATE = false;

		constexpr float TOOLS_NOTIFICATION_MAX_DISPLAY_TIME = 5.0f;
		constexpr float TOOLS_NOTIFICATION_MIN_ALPHA = 0.5f;
		constexpr ImVec2 TOOLS_NOTIFICATION_START_POSITION{ 5.f, 42.f };
		constexpr ImVec2 TOOLS_NOTIFICATION_SIZE{ 250.f, 72.f };

		bool FilterDigitOnly(const std::string& _str);

		namespace widgets
		{
			struct ToggleButton
			{
				ToggleButton(const std::string& _label, const std::function<void()>& _onClick)
					: m_Label(_label)
					, m_OnClick(_onClick)
				{}

				ToggleButton(const std::string& _label, const std::function<void()>& _onClick, bool _active)
					: m_Label(_label)
					, m_OnClick(_onClick)
					, m_Active(_active)
				{}

				void Render()
				{
					if (m_Active)
					{
						ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.7f, 0.21f, 1.f));
						ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.8f, 0.3f, 1.f));
					}
					else
					{
						ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.5f, 0.5f, 0.5f, 0.4f));
						ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.6f, 0.6f, 0.6f, 0.45f));
					}

					if (ImGui::Button(m_Label.c_str()))
					{
						m_Active = !m_Active;

						if (m_OnClick)
							m_OnClick();
					}

					ImGui::PopStyleColor(2);
				}
				
				bool IsActive() const { return m_Active; }

				std::string m_Label = "Basic label";
				std::function<void()> m_OnClick;
				bool m_Active = false;
			};
		}

		/////////////////////////////////////////////////////////
		struct flags
		{
			const int toolbar_window_flags = ImGuiWindowFlags_NoResize |
				ImGuiWindowFlags_NoMove |
				ImGuiWindowFlags_NoCollapse |
				ImGuiWindowFlags_NoTitleBar |
				ImGuiWindowFlags_NoScrollbar |
				ImGuiWindowFlags_NoScrollWithMouse |
				ImGuiWindowFlags_NoBringToFrontOnFocus |
				ImGuiWindowFlags_NoSavedSettings;

			const int message_window_flags = toolbar_window_flags;
		};
		/////////////////////////////////////////////////////////
		struct paths
		{
			const std::string project_root_path_substr = "caspian";

			std::string TrimToProjectRootPath(const std::string& _path)
			{
				std::string path = _path;

				size_t pos = path.find(project_root_path_substr);
				if (pos != std::string::npos)
				{
					path.erase(0, pos);
				}

				return path;
			}
		};

		/////////////////////////////////////////////////////////
		struct styles
		{
			void toolbar_push_combobox_style()
			{
				ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.26f, 0.59f, 0.98f, 0.31f));
				ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0.26f, 0.59f, 0.98f, 0.80f));
				ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(0.26f, 0.59f, 0.98f, 1.00f));
				ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.16f, 0.29f, 0.48f, 0.54f));
				ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(0.26f, 0.59f, 0.98f, 0.40f));
				ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(0.26f, 0.59f, 0.98f, 0.67f));
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.93f, 0.94f, 0.95f, 0.96f));
			}

			void toolbar_pop_combobox_style()
			{
				ImGui::PopStyleColor(7);
			}

			void toolbar_push_button_style()
			{
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.2f, 0.2f, 1.f));
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.3f, 0.3f, 1.f));
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.4f, 0.4f, 0.4f, 1.f));
			}

			void toolbar_pop_button_style()
			{
				ImGui::PopStyleColor(3);
			}

			void push_accept_button_style()
			{
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.8f, 0.0f, 1.0f));
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.0f, 1.0f, 0.0f, 1.0f));
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.0f, 0.6f, 0.0f, 1.0f));
			}

			void pop_accept_button_style()
			{
				ImGui::PopStyleColor(3);
			}

			void push_cancel_button_style()
			{
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.0f, 0.0f, 1.0f));
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.6f, 0.0f, 0.0f, 1.0f));
			}

			void pop_cancel_button_style()
			{
				ImGui::PopStyleColor(3);
			}

			ImVec2 toolbar_button_size = ImVec2(60.f, 20.f);
		};
	};
};