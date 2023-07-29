#pragma once

#include "IWindow.hpp"

#include "Utils.hpp"

namespace Tools_Impl
{
	/////////////////////////////////////////////////////////

	class Manager;
	class NewProjectWindow : public IWindow
	{
		public:
			NewProjectWindow(Manager* _mgr);
			~NewProjectWindow() = default;

			void Render() override;

		private:

			std::string m_projectName;
			std::string m_projectPath;

			// TODO: UUID
			// TODO: Type
	};

	/////////////////////////////////////////////////////////
	//class LoadProjectAction
	//{
	//	public:
	//		
	//		ImGuiLoadProjectAction() = default;
	//		~ImGuiLoadProjectAction() = default;

	//		void operator()();
	//};
};