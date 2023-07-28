#include "engine/pch.hpp"

#include "ImGuiAssetsTools.hpp"
#include "ImGuiTools.hpp"

#include <iostream>
#include <imgui-file-dialog/ImGuiFileDialog.h>

using namespace Tools;

/////////////////////////////////////////////////////////
/* ImGuiImportAssetWindow */
void ImGuiImportAssetWindow::Render()
{
	if (!m_Active)
		return;

	if (ImGui::Begin("Import Asset Window", &m_Active))
	{
		constexpr auto dialog_name = "ImportAssetFileDialog";
		if (ImGui::Button("Select file"))
		{
			ImGuiFileDialog::Instance()->OpenDialog(dialog_name, "Choose File", ".cpp,.h,.hpp", ".");
		}

		if (ImGuiFileDialog::Instance()->Display(dialog_name))
		{
			if (ImGuiFileDialog::Instance()->IsOk())
			{
				const std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
				const std::string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();

				std::cout << "[ImGuiImportAssetWindow] File path name: " << filePathName << "\n";
				std::cout << "[ImGuiImportAssetWindow] File path: " << filePath << "\n";
			}

			ImGuiFileDialog::Instance()->Close();
		}

		if (ImGui::Button("Finish"))
		{
			m_Manager->ShowNotification(ENotificationType::Warning, "Not implemented yet");
		}
	}

	ImGui::End();
}

////////////////////////////////////////////////////////
/* ImGuiAssetListWindow */
void ImGuiAssetListWindow::Render()
{
	if (!m_Active)
		return;

	if (ImGui::Begin("Asset List Window", &m_Active))
	{

	}
	ImGui::End();
}