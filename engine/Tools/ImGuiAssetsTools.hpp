#pragma once

#include "ImGuiIWindow.hpp"

namespace Tools
{
	/////////////////////////////////////////////////////////
	class ImGuiImportAssetWindow : public ImGuiIWindow
	{
	public:

		ImGuiImportAssetWindow(ImGuiManager* _mgr) : ImGuiIWindow(_mgr) {}
		~ImGuiImportAssetWindow() = default;

		void Render() override;
	};

	/////////////////////////////////////////////////////////
	class ImGuiAssetListWindow : public ImGuiIWindow
	{
	public:

		ImGuiAssetListWindow(ImGuiManager* _mgr) : ImGuiIWindow(_mgr) {}
		~ImGuiAssetListWindow() = default;

		void Render() override;
	};

};