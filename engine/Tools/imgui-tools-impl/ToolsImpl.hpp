#pragma once

#include <imgui/imgui.h>

#include "IWindow.hpp"
#include "Utils.hpp"
#include "AssetWindows.hpp"
#include "ProjectWindows.hpp"
#include "NotificationWindows.hpp"

namespace Tools_Impl
{
	/////////////////////////////////////////////////////////
	class Toolbar : public IWindow
	{
		public:

			Toolbar(Manager* _mgr);
			~Toolbar() = default;

			void Render() override;
	};
	
/////////////////////////////////////////////////////////
	enum class ENotificationType
	{
		None,
		Warning,
		Error,
		Success,
		COUNT
	};

	/////////////////////////////////////////////////////////
	class NotificationsManager
	{
		public:

			NotificationsManager(Manager* _mgr);
			~NotificationsManager() = default;

			void Update(float _dt);
			void Render();

			void ShowNotification(ENotificationType _type, std::string_view _msg);
			void ClearNotifications();

			void OnNotificationHidden();

		private:

			// ImGuiNotificationWindows.h
			std::unique_ptr<notifications::WarningNotificationWindow> m_WarningWindow;
			std::unique_ptr<notifications::ErrorNotificationWindow>		m_ErrorWindow;
			std::unique_ptr<notifications::SuccessNotificationWindow> m_SuccessWindow;

			Manager* m_Manager = nullptr;
	};
	
	/////////////////////////////////////////////////////////
	class Manager
	{
		public:

			Manager();
			~Manager() = default;

			void ToggleActiveState();

			void Update(float _dt);
			void Render();

			void ShowNotification(ENotificationType _type, std::string_view _msg);

			void CreateNewProjectRequest(const std::string& _project_name, const std::string& _project_path);
			void LoadProjectRequest(const std::string& _project_name, const std::string& _project_path);

		public:

			// ImGuiAssetsTools.h
			ImportAssetWindow m_ImportAssetWindow;
			AssetsListWindow m_AssetListWindow;

			// ImGuiProjectTools.h
			NewProjectWindow m_NewProjectWindow;
			LoadProjectWindow m_LoadProjectWindow;

			// ImGuiTools.h
			NotificationsManager m_NotificationManager;

		private:

			Toolbar m_Toolbar;

			bool m_Active = utils::TOOLS_INIT_ACTIVE_STATE;

			std::array<unsigned char, 4> m_MagicNumberBytes;
	};
};