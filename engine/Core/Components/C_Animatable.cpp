#include "engine/pch.hpp"

#include "C_Animatable.hpp"

C_Animatable::C_Animatable(GameObject* _owner)
	: IComponent(_owner)
{
	m_sPtrSprite = m_Owner->GetComponent<C_Sprite>();
	m_p_AnimationsController = ApplicationSingleton::Instance().GetEngineController().GetAnimationsController();
}

void C_Animatable::Update(float _dt)
{
	m_p_AnimationsController->Update(_dt);

	if (const Animation* curr_anim = m_p_AnimationsController->GetCurrentAnimation())
	{
		auto& main_instance = ApplicationSingleton::Instance();
		const sf::Texture& new_texture = main_instance.GetEngineController().GetAssetsStorage()->GetConstTexture(curr_anim->GetTextureKey());
		m_sPtrSprite->SetTexture(new_texture);
		m_sPtrSprite->SetTextureRect(m_p_AnimationsController->GetCurrentAnimationFrame()->m_Rect);
	}
}

void C_Animatable::PlayAnimation(const std::string& _name)
{
	StopAnimation();
	m_p_AnimationsController->PlayAnimation(_name);
}

void C_Animatable::StopAnimation()
{
	m_p_AnimationsController->StopAnimation();
}

