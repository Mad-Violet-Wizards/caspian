#pragma once

#include "ImGuiINotificationWindow.hpp"

namespace Tools
{
	class ImGuiManager;

	namespace notifications
	{
		class WarningWindow : public ImGuiINotificationWindow
		{
			public:
				
				WarningWindow(ImGuiManager* _mgr);
				~WarningWindow() = default;

				void Render() override;
		};

		class ErrorWindow : public ImGuiINotificationWindow
		{
			public:

				ErrorWindow(ImGuiManager* _mgr);
				~ErrorWindow() = default;

				void Render() override;
		};
	}
}