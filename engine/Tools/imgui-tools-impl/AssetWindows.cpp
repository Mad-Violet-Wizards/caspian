#include "engine/pch.hpp"

#include "AssetWindows.hpp"
#include "ToolsImpl.hpp"
#include "engine/Core/Projects.hpp"

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
			Projects::Manager* project_manager = ApplicationSingleton::Instance().GetProjectsManager();
			auto project_path = project_manager->GetCurrentProject().m_ProjectPath;
			project_path += "\\" + project_manager->GetCurrentProject().m_ProjectName + "\\";

			ImGuiFileDialog::Instance()->OpenDialog(dest_dialog_name, "Choose Dest", nullptr, project_path);
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

	auto copy_file_async = [](const std::string& _src, const std::string& _dest, const std::function<void(bool)>& _callback)
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
			std::string dest_relative_path;
			bool copy_success = false;

			auto& engine_module = ApplicationSingleton::Instance().GetEngineController();
			fs::IFileSystem* resources_fs = engine_module.GetFilesystemManager()->Get("resources");

			copy_file_async(m_srcPath, m_destPath, [this, resources_fs, &copy_success,  &dest_relative_path]
																							(bool _success)
				{
					if (_success)
					{
						const std::string full_dest_path = m_destPath + "\\" + m_srcFileName;
						dest_relative_path = full_dest_path.substr(resources_fs->GetPath().size() + 1);
						const bool registered = resources_fs->RegisterFile(dest_relative_path);

						copy_success = registered;
					}
					else
					{
						m_Manager->ShowNotification(ENotificationType::Error, "Asset file couldn't be copied.");
					}
			});

			if (copy_success)
			{
				using FileSharedPtr = std::shared_ptr<fs::IFile>;

				auto fill_assets_storage_single_resource_async = [this, dest_relative_path, resources_fs, &engine_module]
																												 (std::function<void(FileSharedPtr _to_close)> _callback)
					{
						auto future_result = std::async(std::launch::async, 
																						[this, dest_relative_path, resources_fs, &engine_module, _callback]
																						()
							{
								using namespace fs::io;
								FileSharedPtr file = resources_fs->OpenFile(dest_relative_path, OpenMode::In | OpenMode::Binary);
								Assets::Storage* assets_storage = engine_module.GetAssetsStorage();
								assets_storage->LoadResourceAcceptableType(file.get());

								_callback(file);
							});
					};

				fill_assets_storage_single_resource_async([&](FileSharedPtr _to_close)
					{
						const bool file_closed_ok = resources_fs->CloseFile(_to_close);
						m_Manager->ShowNotification(ENotificationType::Success, "Asset added succesfully.");

						if (!file_closed_ok)
							std::cerr << "Error: file " << _to_close->GetPath() << " couldn't be closed.\n";
					});
			}
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

	Assets::Storage* assets_storage = ApplicationSingleton::Instance().GetEngineController().GetAssetsStorage();
	size_t assets_storage_count = assets_storage->GetTotalResourcesCount();

	// TODO: It might be good to do some benchmarking as now 
	// we need to do O(n) operation every time we add/remove any resource.
	// I wonder how quick it might became an problem.

	if (assets_storage_count != m_RegisteredAssets.size())
	{
		m_RegisteredAssets.clear();

		std::vector<std::string> registered_assets;
		registered_assets.reserve(assets_storage_count);

		std::ranges::move(assets_storage->GetTexturesKeys(), std::back_inserter(registered_assets));
		std::ranges::move(assets_storage->GetFontKeys(),		 std::back_inserter(registered_assets));

		fs::IFileSystem* resource_fs = ApplicationSingleton::Instance().GetEngineController().GetFilesystemManager()->Get("resources");

		for (const std::string& asset_name : registered_assets)
		{
			const std::string asset_path = resource_fs->GetAbsoluteFilePath(asset_name);

			Internal_AssetTableData d;
			d.m_Name = asset_name.substr(asset_name.find_last_of("\\/") + 1);
			d.m_RelativePath = asset_name;
			d.m_AbsolutePath = asset_path;

			if (assets_storage->IsTexture(asset_name))
				d.m_Type = "Texture";
			else if (assets_storage->IsFont(asset_name))
				d.m_Type = "Font";
			else
				d.m_Type = "Unknown";

			m_RegisteredAssets.push_back(d);
		}
	}


	if (m_AssetSelectedToDelete.has_value())
	{
		std::cout << "Asset selected to delete: " << m_AssetSelectedToDelete.value().m_Name << "\n";

		auto fn_async_delete = [&]()
			{
				auto future_result = std::async(std::launch::async, [&]()
					{
						const Internal_AssetToDelete& asset_to_delete = m_AssetSelectedToDelete.value();

						auto file_etype = fs::IFile::ETypeFromString(asset_to_delete.m_Type);
						Assets::Storage* assets_storage = ApplicationSingleton::Instance().GetEngineController().GetAssetsStorage();
						assets_storage->DeleteResource(asset_to_delete.m_Name, file_etype);

						fs::IFileSystem* resource_fs = ApplicationSingleton::Instance().GetEngineController().GetFilesystemManager()->Get("resources");
						const bool file_deleted_successfully = resource_fs->RemoveFile(asset_to_delete.m_Name);

						asset_to_delete.m_OnDelete(file_deleted_successfully);
					});
			};

		fn_async_delete();
		m_AssetSelectedToDelete.reset();
	}
	if (!m_SearchPhrase.empty())
	{
		auto fn_str_to_lower_by_ref = [](std::string& _str) { std::transform(_str.begin(), _str.end(), _str.begin(), [](unsigned char c) { return std::tolower(c); }); };

		auto fn_search = [](std::string_view _search_phrase, std::string_view _txt_to_filter) -> bool
		{
				auto boyer_moore_searcher = std::boyer_moore_searcher(_search_phrase.begin(), _search_phrase.end());
				return std::search(_txt_to_filter.begin(), _txt_to_filter.end(), boyer_moore_searcher) != _txt_to_filter.end();
		};

		auto lower_search_phrase = m_SearchPhrase;
		fn_str_to_lower_by_ref(lower_search_phrase);

		for (auto& asset_data : m_RegisteredAssets)
		{
			std::string txt_to_filter = asset_data.m_Name + asset_data.m_AbsolutePath;
			fn_str_to_lower_by_ref(txt_to_filter);
			fn_search(lower_search_phrase, txt_to_filter) ? asset_data.m_bSearchFlag = true : asset_data.m_bSearchFlag = false;
		}
	}
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

		if(ImGui::BeginTable("Assets List", 6, ImGuiTableFlags_Resizable | ImGuiTableFlags_ScrollY | ImGuiTableFlags_RowBg))
		{
			ImGui::TableNextColumn();
			ImGui::Text("Number");
			ImGui::TableNextColumn();
			ImGui::Text("Asset type");
			ImGui::TableNextColumn();
			ImGui::Text("Asset name");
			ImGui::TableNextColumn();
			ImGui::Text("Asset relative path / Assets Storage Key");
			ImGui::TableNextColumn();
			ImGui::Text("Asset absolute path");
			ImGui::TableNextColumn();
			ImGui::Text("Actions");
			ImGui::TableNextRow();

			for (auto i = 0; i < m_RegisteredAssets.size(); ++i)
			{
				auto& asset = m_RegisteredAssets[i];

				if (!m_SearchPhrase.empty())
					if (!asset.m_bSearchFlag)
						continue;

				ImGui::TableNextColumn();
				ImGui::Text("%d", i+1);
				ImGui::TableNextColumn();
				ImGui::Text(asset.m_Type.c_str());
				ImGui::TableNextColumn();
				ImGui::Text(asset.m_Name.c_str());
				ImGui::TableNextColumn();
				ImGui::Text(asset.m_RelativePath.c_str());
				ImGui::TableNextColumn();
				ImGui::Text(asset.m_AbsolutePath.c_str());
				ImGui::TableNextColumn();
				if (m_ActionListener)
				{
					ImGui::PushID(i);

					if (ImGui::Button("Delete"))
					{
						auto on_delete = [&](bool _success)
							{
								if (_success)
									m_Manager->ShowNotification(ENotificationType::Success, "Asset deleted successfully.");
								else
									m_Manager->ShowNotification(ENotificationType::Error, "Asset couldn't be deleted.");
							};

						Internal_AssetToDelete asset_to_delete{ asset.m_Name, asset.m_Type, on_delete };
						m_AssetSelectedToDelete.emplace(asset_to_delete);
					}
					ImGui::SameLine();
					if (ImGui::Button("Select"))
					{
						SelectedAssetData data;
						data.m_RelativePath = asset.m_RelativePath;
						NotifyAssetSelected(data);
					}
					ImGui::PopID();
				}
				ImGui::TableNextRow();
			}

			ImGui::EndTable();
		}
	}
	ImGui::End();
}

void AssetsListWindow::NotifyAssetSelected(const SelectedAssetData& _data)
{
	if (!m_ActionListener)
		return;

	m_ActionListener->OnAssetSelected(_data);

	m_ActionListener = nullptr;
}
