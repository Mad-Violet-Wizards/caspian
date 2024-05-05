#pragma once

#include "IWindow.hpp"
#include "engine/Core/Animations.hpp"

namespace Tools_Impl
{
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

			AnimationPreviewWindow(Manager* _mgr) : IWindow(_mgr) {}

			void Render() override;
			void Update(float _dt);
	};

	class NewAnimationWindow : public IWindow
	{
		public:


			struct AnimationInternalData
			{
				float m_FrameTime;
				sf::IntRect m_FrameRect;
			};
		
			NewAnimationWindow(Manager* _mgr, AnimationPreviewWindow* _anim_preview_window) 
				: IWindow(_mgr)
				, m_PreviewWindow(_anim_preview_window)
				, m_AnimationType(EAnimationType::Unknown)
				, m_AnimationDirection(EAnimationDirection::None)
			{}

			void Render() override;
			void Update(float _dt);

	private:

			std::string m_AnimationName;
			std::string m_AnimationTexturePath;
			std::string m_AnimationFrameCount;
			std::vector<AnimationInternalData> m_AnimationData;
			EAnimationType m_AnimationType;
			EAnimationDirection m_AnimationDirection;

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