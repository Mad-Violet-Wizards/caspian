#pragma once

#include "INotificationWindow.hpp"

namespace Tools_Impl
{
	class Manager;

	namespace notifications
	{
		class WarningWindow : public INotificationWindow
		{
			public:
				
				WarningWindow(Manager* _mgr);
				~WarningWindow() = default;

				void Render() override;
		};

		class ErrorWindow : public INotificationWindow
		{
			public:

				ErrorWindow(Manager* _mgr);
				~ErrorWindow() = default;

				void Render() override;
		};
	}
}