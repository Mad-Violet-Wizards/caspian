#pragma once

#include <imgui/imgui.h>

namespace Tools
{
	constexpr bool INIT_ACTIVE_STATE = false;

	/////////////////////////////////////////////////////////
	namespace utils
	{
		/////////////////////////////////////////////////////////
		struct flags
		{
			int toolbar_window_flags = ImGuiWindowFlags_NoResize |
				ImGuiWindowFlags_NoMove |
				ImGuiWindowFlags_NoCollapse |
				ImGuiWindowFlags_NoTitleBar |
				ImGuiWindowFlags_NoScrollbar |
				ImGuiWindowFlags_NoScrollWithMouse |
				ImGuiWindowFlags_NoBringToFrontOnFocus |
				ImGuiWindowFlags_NoSavedSettings |
				ImGuiWindowFlags_MenuBar;
		};

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

			void toolbar_push_button_style()
			{
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.2f, 0.2f, 1.f));
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.3f, 0.3f, 1.f));
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.4f, 0.4f, 0.4f, 1.f));
			}

			void toolbar_pop_combobox_style()
			{
				ImGui::PopStyleColor(7);
			}

			void toolbar_pop_button_style()
			{
				ImGui::PopStyleColor(3);
			}

			ImVec2 toolbar_button_size = ImVec2(60.f, 20.f);
		};
	};

	/////////////////////////////////////////////////////////
	class IImGuiWidget
	{
		public:

			IImGuiWidget() = default;
			virtual ~IImGuiWidget() = default;

			virtual void Update(float _dt) = 0;
			virtual void Render() = 0;

		public:

			bool m_Active = INIT_ACTIVE_STATE;
	};

	/////////////////////////////////////////////////////////

	class ImGuiManager;
	class ImGuiToolbar : public IImGuiWidget
	{
		public:

			ImGuiToolbar(ImGuiManager* _mgr);
			~ImGuiToolbar() = default;

			void Update(float _dt) override;
			void Render() override;

		private:

			ImGuiManager* m_Manager = nullptr;
	};
	/////////////////////////////////////////////////////////
	class ImGuiImportAssetWindow : public IImGuiWidget
	{
		public:

			ImGuiImportAssetWindow() = default;
			~ImGuiImportAssetWindow() = default;

			void Update(float _dt) override;
			void Render() override;
	};

	/////////////////////////////////////////////////////////
	class ImGuiManager
	{
		public:

			ImGuiManager();
			~ImGuiManager() = default;

			void ToggleActiveState();

			void Update(float _dt);
			void Render();

		public:

			ImGuiImportAssetWindow m_ImportAssetWindow;

		private:

			ImGuiToolbar m_Toolbar;

			bool m_Active = INIT_ACTIVE_STATE;
	};
};