#pragma once

#include "ImGuiIWindow.hpp"

#include "ImGuiUtils.hpp"


namespace Tools
{
	/////////////////////////////////////////////////////////

	class ImGuiManager;
	class ImGuiNewProjectWindow : public ImGuiIWindow
	{
		public:
			ImGuiNewProjectWindow(ImGuiManager* _mgr);
			~ImGuiNewProjectWindow() = default;

			void Render() override;

		private:

			std::string m_projectName;
			std::string m_projectPath;

			// TODO: UUID
			// TODO: Type
	};

	/////////////////////////////////////////////////////////
	class ImGuiLoadProjectAction
	{
		public:
			
			ImGuiLoadProjectAction() = default;
			~ImGuiLoadProjectAction() = default;

			void operator()();
	};
};