#pragma once

#include "engine/Core/IComponent.hpp"

class C_Animatable : public IComponent
{
	public:

		C_Animatable(GameObject* _owner);

		void Update(float _dt) override;
		void LateUpdate(float _dt) override {};

		void PlayAnimation(const std::string& _name);
		void StopAnimation();

	private:

		AnimationsController* m_p_AnimationsController;
		std::shared_ptr<C_Sprite> m_sPtrSprite;
};