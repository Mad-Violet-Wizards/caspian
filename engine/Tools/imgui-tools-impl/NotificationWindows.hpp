#pragma once

#include "INotificationWindow.hpp"

namespace Tools_Impl
{
	class Manager;

	namespace notifications
	{
		class WarningNotificationWindow : public INotificationWindow
		{
			public:
				
				WarningNotificationWindow(Manager* _mgr);
				~WarningNotificationWindow() = default;

				void Render() override;
		};

		class ErrorNotificationWindow : public INotificationWindow
		{
			public:

				ErrorNotificationWindow(Manager* _mgr);
				~ErrorNotificationWindow() = default;

				void Render() override;
		};
	}
}