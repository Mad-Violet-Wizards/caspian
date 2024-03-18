#pragma once

#include "IWindow.hpp"

namespace Tools_Impl
{
	/////////////////////////////////////////////////////////
	class CameraToolbox : public IWindow
	{
		public:

			CameraToolbox(Manager* _mgr);
			~CameraToolbox() = default;

			void Update(float _dt);
			void Render();

			void OnMoveCameraToggled();

		private:

			utils::widgets::ToggleButton m_MoveCameraButton;
	};
	/////////////////////////////////////////////////////////
	class LevelToolbox : public IWindow
	{
		public:

			LevelToolbox(Manager* _mgr);
			~LevelToolbox() = default;

			void Update(float _dt);
			void Render();

			void OnPlaceTileToggled();
			void OnEraseTileToggled();

		private:

			utils::widgets::ToggleButton m_PlaceTileButton;
			utils::widgets::ToggleButton m_EraseTileButton;

	};

	/////////////////////////////////////////////////////////
	class ToolboxWindow : public IWindow
	{
		public:

			ToolboxWindow(Manager* _mgr);
			~ToolboxWindow() = default;

			void Update(float _dt) override;
			void Render() override;

		private:


			int m_Fps = 0;
			int m_FpsCountLimit = 60; // Collect FPS value for 60 frames.

			std::vector<int> m_SavedFps;

			CameraToolbox m_CameraToolbox;
			LevelToolbox m_LevelToolbox;
	};
}