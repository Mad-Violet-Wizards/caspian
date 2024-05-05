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


	class AnimationListWindow : public IWindow
	{
	public:

		AnimationListWindow(Manager* _mgr) : IWindow(_mgr) {}

		void Render() override;
		void Update(float _dt);
	};

	class AnimationPreviewWindow : public IWindow
	{
		public:

			AnimationPreviewWindow(Manager* _mgr) 
				: IWindow(_mgr)
				, m_AnimationTexturePreview(nullptr)
			{}

			void Render() override;
			void Update(float _dt);

			void SetTexture(const std::string& _texture_path);
			void SyncNewAnimationData(const std::vector<AnimationFrameInternalData>& _frame_internal_data);

		private:

			const sf::Texture* m_AnimationTexturePreview;

			std::vector<AnimationFramePreviewInternalData> m_AnimationFrameData;
	};


	class NewAnimationWindow : public IWindow, public IAssetsTableActionsListener
	{
	public:

		NewAnimationWindow(Manager* _mgr, AnimationPreviewWindow* _anim_preview_window)
			: IWindow(_mgr)
			, m_PreviewWindow(_anim_preview_window)
			, m_AnimationType(EAnimationType::Unknown)
			, m_AnimationDirection(EAnimationDirection::None)
			, m_PrevAnimationFrameCountInt(0)
		{}

		void Render() override;
		void Update(float _dt);

		void OnAssetSelected(const SelectedAssetData& data) override;

	private:

		std::string m_AnimationName;
		std::string m_AnimationTexturePath;
		std::string m_AnimationFrameCount;
		std::vector<AnimationFrameInternalData> m_AnimationData;
		EAnimationType m_AnimationType;
		EAnimationDirection m_AnimationDirection;
		unsigned int m_PrevAnimationFrameCountInt;

		AnimationPreviewWindow* m_PreviewWindow;

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

	class AnimationEditortWindow : public IWindow
	{
		public:

			AnimationEditortWindow(Manager* _mgr) 
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