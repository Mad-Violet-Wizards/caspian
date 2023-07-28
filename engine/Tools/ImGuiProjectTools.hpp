#pragma once

#include "engine/Tools/IImGuiWidget.hpp"

#include "engine/Tools/ImGuiUtils.hpp"


namespace Tools
{
	/////////////////////////////////////////////////////////

	class ImGuiManager;
	class ImGuiNewProjectWindow : public IImGuiWidget
	{
		public:
			ImGuiNewProjectWindow(ImGuiManager* _mgr);
			~ImGuiNewProjectWindow() = default;

			void Render() override;

		private:

			std::string m_projectName;
			std::string m_projectPath;

			ImGuiManager* m_Manager;
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