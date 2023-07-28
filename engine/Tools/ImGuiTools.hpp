#pragma once

#include <imgui/imgui.h>

#include "ImGuiIWindow.hpp"
#include "ImGuiUtils.hpp"
#include "ImGuiAssetsTools.hpp"
#include "ImGuiProjectTools.hpp"
#include "ImGuiNotificationWindows.hpp"

namespace Tools
{
	/////////////////////////////////////////////////////////
	class ImGuiToolbar : public ImGuiIWindow
	{
		public:

			ImGuiToolbar(ImGuiManager* _mgr);
			~ImGuiToolbar() = default;

			void Render() override;
	};
	
/////////////////////////////////////////////////////////
	enum class ENotificationType
	{
		None,
		Warning,
		Error,
		COUNT
	};

	/////////////////////////////////////////////////////////
	class ImGuiNotificationManager
	{
		public:

			ImGuiNotificationManager(ImGuiManager* _mgr);
			~ImGuiNotificationManager() = default;

			void Update(float _dt);
			void Render();

			void ShowNotification(ENotificationType _type, std::string_view _msg);
			void ClearNotifications();

			void OnNotificationHidden();

		private:

			// ImGuiNotificationWindows.h
			std::unique_ptr<notifications::WarningWindow> m_WarningWindow;
			std::unique_ptr<notifications::ErrorWindow>		m_ErrorWindow;

			ImGuiManager* m_Manager = nullptr;
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

			void ShowNotification(ENotificationType _type, std::string_view _msg);

			void OnCreateNewProject(std::string_view _project_name, std::string_view _project_path);

		public:

			// ImGuiAssetsTools.h
			ImGuiImportAssetWindow m_ImportAssetWindow;
			ImGuiAssetListWindow m_AssetListWindow;

			// ImGuiProjectTools.h
			ImGuiNewProjectWindow m_NewProjectWindow;

			// ImGuiTools.h
			ImGuiNotificationManager m_NotificationManager;

		private:

			ImGuiToolbar m_Toolbar;

			bool m_Active = utils::TOOLS_INIT_ACTIVE_STATE;
	};
};