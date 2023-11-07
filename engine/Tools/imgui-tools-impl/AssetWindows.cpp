#include "engine/pch.hpp"

#include "AssetWindows.hpp"
#include "ToolsImpl.hpp"
#include "game/Application.hpp"

#include <iostream>
#include <imgui-file-dialog/ImGuiFileDialog.h>
#include <imgui/misc/cpp/imgui_stdlib.h>

using namespace Tools_Impl;

/////////////////////////////////////////////////////////
/* ImportAssetWindow */
void ImportAssetWindow::Render()
{
	if (!m_Active)
		return;

	if (ImGui::Begin("Import Asset Window", &m_Active))
	{
		constexpr auto src_dialog_name = "ImportAssetFileDialog";
		if (ImGui::Button("Select file"))
		{
			ImGuiFileDialog::Instance()->OpenDialog(src_dialog_name, "Choose File", ".png,.wav,.mp3,.ttf", ".");
		}

		if (ImGuiFileDialog::Instance()->Display(src_dialog_name))
		{
			if (ImGuiFileDialog::Instance()->IsOk())
			{
				m_srcPath = ImGuiFileDialog::Instance()->GetFilePathName();
				m_srcFileName = ImGuiFileDialog::Instance()->GetCurrentFileName();
			}

			ImGuiFileDialog::Instance()->Close();
		}

		const std::string fmt_src = std::format("Selected file: {}", m_srcPath);
		ImGui::Text(fmt_src.c_str());

		constexpr auto dest_dialog_name = "DestAssetFileDialog";
		if (ImGui::Button("Select dest folder"))
		{
			auto& engine_module = ApplicationSingleton::Instance().GetEngineModule();
			auto project_path = engine_module.GetCurrentProject().m_ProjectPath;
			project_path += "\\" + engine_module.GetCurrentProject().m_ProjectName;

			ImGuiFileDialog::Instance()->OpenDialog(dest_dialog_name, "Choose Dest", ".", project_path);
		}

		if (ImGuiFileDialog::Instance()->Display(dest_dialog_name))
		{
			if (ImGuiFileDialog::Instance()->IsOk())
			{
				m_destPath = ImGuiFileDialog::Instance()->GetCurrentPath();
			}

			ImGuiFileDialog::Instance()->Close();
		}
	}

	const std::string fmt_dest = std::format("Selected (dest) path: {}", m_destPath);
	ImGui::Text(fmt_dest.c_str());

	const bool import_button_enabled = !m_srcPath.empty() && !m_destPath.empty();

	import_button_enabled ? ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 1.0f, 0.0f, 1.0f)) : ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));

	auto copy_file_async = [](const std::string& _src, const std::string& _dest, std::function<void(bool)> _callback)
	{
			auto future_result = std::async(std::launch::async, [_src, _dest, _callback]()
			{
					bool success = false;
					try
					{
						std::filesystem::copy(_src, _dest);
						success = true;
					}
					catch (const std::filesystem::filesystem_error& e)
					{
						std::cerr << "Error: " << e.what() << "\n";
					}

					_callback(success);
			});
	};

	if (ImGui::Button("Import")) 
	{
		if (import_button_enabled)
		{
			copy_file_async(m_srcPath, m_destPath, [this](bool _success)
				{
					if (_success)
					{
						auto& engine_module = ApplicationSingleton::Instance().GetEngineModule();
						fs::IFileSystem* resources_fs = engine_module.GetFilesystemManager()->Get("resources");
						const std::string full_dest_path = m_destPath + "\\" + m_srcFileName;
						const std::string relative_path = full_dest_path.substr(resources_fs->GetPath().size() + 1);
						const bool registered = resources_fs->RegisterFile(relative_path);

						if (registered) 
							m_Manager->ShowNotification(ENotificationType::Success, "Asset imported successfuly.");
						else
							m_Manager->ShowNotification(ENotificationType::Success, "Asset couldn't be registered in fs.");
					}
					else
					{
						m_Manager->ShowNotification(ENotificationType::Error, "Asset file couldn't be copied.");
					}
			});
		}
	}
	
	ImGui::PopStyleColor();

	ImGui::End();
}

////////////////////////////////////////////////////////
/* AssetListWindow */
void AssetsListWindow::Update(float _dt)
{
	if (!m_Active)
		return;

	Assets::Storage* assets_storage = ApplicationSingleton::Instance().GetEngineModule().GetAssetsStorage();
	size_t assets_storage_count = assets_storage->GetTotalResourcesCount();

	if (assets_storage_count != m_RegisteredAssets.size())
	{
		m_RegisteredAssets.clear();

		std::vector<std::string> registered_assets;
		registered_assets.reserve(assets_storage_count);

		std::ranges::move(assets_storage->GetTexturesKeys(), std::back_inserter(registered_assets));
		std::ranges::move(assets_storage->GetFontKeys(),		 std::back_inserter(registered_assets));

		fs::IFileSystem* resource_fs = ApplicationSingleton::Instance().GetEngineModule().GetFilesystemManager()->Get("resources");

		for (const std::string& asset_name : registered_assets)
		{
			const std::string asset_path = resource_fs->GetAbsoluteFilePath(asset_name);

			Internal_AssetTableData d;
			d.m_Name = asset_name;
			d.m_Path = asset_path;

			if (assets_storage->IsTexture(asset_name))
				d.m_Type = "Texture";
			else if (assets_storage->IsFont(asset_name))
				d.m_Type = "Font";
			else
				d.m_Type = "Unknown";

			m_RegisteredAssets.push_back(d);
		}
	}

	//if (!m_SearchPhrase.empty())
	//{
		// TODO: Do searching, std::boyer_moore_searcher.
	//}
}

void AssetsListWindow::Render()
{
	if (!m_Active)
		return;

	if (ImGui::Begin("Asset List Window", &m_Active, ImGuiTableFlags_Resizable))
	{
		ImGui::InputText("Search phrase: ", &m_SearchPhrase);
		
		if (ImGui::Button("Exit"))
			m_Active = false;

		if(ImGui::BeginTable("Assets List", 5, ImGuiTableFlags_Resizable | ImGuiTableFlags_ScrollY | ImGuiTableFlags_RowBg))
		{
			ImGui::TableNextColumn();
			ImGui::Text("Number");
			ImGui::TableNextColumn();
			ImGui::Text("Asset name");
			ImGui::TableNextColumn();
			ImGui::Text("Asset type");
			ImGui::TableNextColumn();
			ImGui::Text("Asset path");
			ImGui::TableNextColumn();
			ImGui::Text("Actions");
			ImGui::TableNextRow();

			for (auto i = 0; i < m_RegisteredAssets.size(); ++i)
			{
				auto& asset = m_RegisteredAssets[i];

				ImGui::TableNextColumn();
				ImGui::Text("%d", i+1);
				ImGui::TableNextColumn();
				ImGui::Text(asset.m_Name.c_str());
				ImGui::TableNextColumn();
				ImGui::Text(asset.m_Type.c_str());
				ImGui::TableNextColumn();
				ImGui::Text(asset.m_Path.c_str());
				ImGui::TableNextColumn();
				ImGui::Text("...");
				ImGui::TableNextRow();
			}

			ImGui::EndTable();
		}
	}
	ImGui::End();
}