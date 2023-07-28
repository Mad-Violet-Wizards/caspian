#pragma once

#include <imgui/imgui.h>

#include "engine/Tools/IImGuiWidget.hpp"
#include "engine/Tools/ImGuiUtils.hpp"

#include "engine/Tools/ImGuiAssetsTools.hpp"
#include "engine/Tools/ImGuiProjectTools.hpp"

namespace Tools
{
	/////////////////////////////////////////////////////////
	class ImGuiManager;
	class ImGuiToolbar : public IImGuiWidget
	{
		public:

			ImGuiToolbar(ImGuiManager* _mgr);
			~ImGuiToolbar() = default;

			void Render() override;

		private:

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

		public:
			
			void OnCreateNewProject(const std::string_view _project_name, const std::string_view _project_path);

		public:

			// ImGuiAssetsTools.h
			ImGuiImportAssetWindow m_ImportAssetWindow;
			ImGuiAssetListWindow m_AssetListWindow;

			// ImGuiProjectTools.h
			ImGuiNewProjectWindow m_NewProjectWindow;

		private:

			ImGuiToolbar m_Toolbar;

			bool m_Active = false;
	};
};