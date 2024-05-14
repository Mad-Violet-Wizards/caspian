#pragma once

#include "IWindow.hpp"
#include "engine/Core/Animations.hpp"

namespace Tools_Impl
{
	struct AnimationFrameInternalData
	{
		std::string m_FrameTime;
		std::string m_FrameRectX;
		std::string m_FrameRectY;
		std::string m_FrameRectWidth;
		std::string m_FrameRectHeight;
	};

	struct AnimationFramePreviewInternalData
	{
		std::string m_Label;
		float m_FrameTime;
		sf::IntRect m_Rect;
	};
	
	enum class EAnimationPreviewOwner
	{
		None,
		NewAnimWindow,
		EditAnimWindow
	};

	class AnimationListWindow : public IWindow
	{
		public:

			AnimationListWindow(Manager* _mgr) : IWindow(_mgr) {}

			void Render() override;
			void Update(float _dt);
	};

	using AnimationPreviewComboScalePair = std::pair<std::string, float>;
	using AnimationPreviewComboSlowdownPair = std::pair<std::string, float>;

	class AnimationPreviewWindow : public IWindow
	{
		public:

			AnimationPreviewWindow(Manager* _mgr)
				: IWindow(_mgr)
				, m_AnimationTexturePreview(nullptr)
				, m_PlayButton("Play", std::bind(&AnimationPreviewWindow::OnPlayButtonToggled, this), false)
				, m_StopButton("Stop", std::bind(&AnimationPreviewWindow::OnStopButtonToggled, this), false)
				, m_PauseButton("Pause", std::bind(&AnimationPreviewWindow::OnPauseButtonToggled, this), false)
				, m_CurrentAnimationFrameIndex(0)
				, m_CurrentFrameTime(0.0f)
				, m_CurrentScalesArrayIndex(2)
				, m_CurrentSpeedValuesArrayIndex(5)
				, m_AnimationPreviewOwner(EAnimationPreviewOwner::None)
				, m_CurrentAnimationType(EAnimationType::Unknown)
				, m_Playing(false)
			{}

			void Render() override;
			void Update(float _dt);

			void SetTexture(const std::string& _texture_path);

			void SetAnimationPreviewOwner(EAnimationPreviewOwner _owner) { m_AnimationPreviewOwner = _owner; }
			void CreatePreviewDataFromFrameInternals(const std::vector<AnimationFrameInternalData>& _frame_internal_data);
			void SetAnimationType(EAnimationType _type) { m_CurrentAnimationType = _type; }

			void OnAnimationFinished();

			void OnPlayButtonToggled();
			void OnStopButtonToggled();
			void OnPauseButtonToggled();

		private:

			const sf::Texture* m_AnimationTexturePreview;

			utils::widgets::ToggleButton m_PlayButton;
			utils::widgets::ToggleButton m_StopButton;
			utils::widgets::ToggleButton m_PauseButton;

			EAnimationPreviewOwner m_AnimationPreviewOwner;

			std::array<AnimationPreviewComboScalePair, 9> m_AnimationScalesArray = {
				AnimationPreviewComboScalePair("x0.25", 0.25f),
				AnimationPreviewComboScalePair("x0.5", 0.5f),
				AnimationPreviewComboScalePair("x1", 1.f),
				AnimationPreviewComboScalePair("x2", 2.f),
				AnimationPreviewComboScalePair("x4", 4.f),
				AnimationPreviewComboScalePair("x8", 8.f),
				AnimationPreviewComboScalePair("x16", 16.f),
				AnimationPreviewComboScalePair("x32", 32.f),
				AnimationPreviewComboScalePair("x64", 64.f),
			};

			std::array<AnimationPreviewComboSlowdownPair, 10> m_AnimationSpeedValuesArray = {
				AnimationPreviewComboSlowdownPair("x32", 32.f),
				AnimationPreviewComboSlowdownPair("x16", 16.f),
				AnimationPreviewComboSlowdownPair("x8", 8.f),
				AnimationPreviewComboSlowdownPair("x4", 4.f),
				AnimationPreviewComboSlowdownPair("x2", 2.f),
				AnimationPreviewComboSlowdownPair("x1", 1.f),
				AnimationPreviewComboSlowdownPair("x0.75", 0.75f),
				AnimationPreviewComboSlowdownPair("x0.5", .5f),
				AnimationPreviewComboSlowdownPair("x0.25", .25f),
				AnimationPreviewComboSlowdownPair("x0.1", .1),
			};

			EAnimationType m_CurrentAnimationType;
			unsigned int m_CurrentScalesArrayIndex;
			unsigned int m_CurrentSpeedValuesArrayIndex;
			unsigned int m_CurrentAnimationFrameIndex;
			float m_CurrentFrameTime;
			bool m_Playing;

			std::vector<AnimationFramePreviewInternalData> m_AnimationFrameData;
	};

	// TODO: Continue here, merge NewAnimationWindow & EditAnimationWindow into one AnimationPropertiesWindow.
	class AnimationPropertiesWindow : public IWindow, public IAssetsTableActionsListener
	{
	public:

		AnimationPropertiesWindow(Manager* _mgr, AnimationPreviewWindow* _anim_preview_window)
			: IWindow(_mgr)
			, m_PreviewWindow(_anim_preview_window)
			, m_AnimationType(EAnimationType::Unknown)
			, m_PrevAnimationFrameCountInt(0)
		{}

		void Render() override;
		void Update(float _dt);

		void OnAssetSelected(const SelectedAssetData& data) override;
		const std::vector<AnimationFrameInternalData>& GetAnimationData() const { return m_AnimationData; }
		EAnimationType GetAnimationType() const { return m_AnimationType; }

	private:

		std::string m_AnimationName;
		std::string m_AnimationTexturePath;
		std::string m_AnimationFrameCount;
		std::vector<AnimationFrameInternalData> m_AnimationData;
		EAnimationType m_AnimationType;
		unsigned int m_PrevAnimationFrameCountInt;

		std::array<EAnimationType, 4> m_AnimationTypes = {
			EAnimationType::Unknown,
			EAnimationType::Loop,
			EAnimationType::OneShot,
			EAnimationType::PingPong
		};

		AnimationPreviewWindow* m_PreviewWindow;

	};

	class NewAnimationWindow : public IWindow
	{
		public:

			NewAnimationWindow(Manager* _mgr, AnimationPreviewWindow* _anim_preview_window) 
				: IWindow(_mgr)
				, m_PreviewWindow(_anim_preview_window)
				, m_PlayInPreview("Play in preview", std::bind(&NewAnimationWindow::OnPlayInPreviewButtonToggled, this), false)
				, m_AnimationPropertiesWindow(_mgr, _anim_preview_window)
			{}

			void Render() override;
			void Update(float _dt);

			void OnPlayInPreviewButtonToggled();

		private:

			utils::widgets::ToggleButton m_PlayInPreview;
			AnimationPreviewWindow* m_PreviewWindow;
			AnimationPropertiesWindow m_AnimationPropertiesWindow;
	};

	class EditAnimationWindow : public IWindow
	{
		public:

				EditAnimationWindow(Manager* _mgr, AnimationPreviewWindow* _anim_preview_window) 
					: IWindow(_mgr)
				  , m_PreviewWindow(_anim_preview_window)
				{}

				void Render() override;
				void Update(float _dt);

		private:
			
				AnimationPreviewWindow* m_PreviewWindow;
	};

	class AnimationEditorWindow : public IWindow
	{
		public:

			AnimationEditorWindow(Manager* _mgr) 
				: IWindow(_mgr)
				, m_AnimationPreviewWindow(_mgr)
				, m_AnimationListWindow(_mgr)
			  , m_NewAnimationWindow(_mgr, &m_AnimationPreviewWindow)
			  , m_EditAnimationWindow(_mgr, &m_AnimationPreviewWindow)
			{}

			void Render() override;
			void Update(float _dt);

		private:

			AnimationPreviewWindow m_AnimationPreviewWindow;
			AnimationListWindow m_AnimationListWindow;
			EditAnimationWindow m_EditAnimationWindow;
			NewAnimationWindow m_NewAnimationWindow;
	};
};