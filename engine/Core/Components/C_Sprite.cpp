#include "engine/pch.hpp"

#include "C_Sprite.hpp"

C_Sprite::C_Sprite(GameObject* _owner)
	: IComponent(_owner)
{

}

void C_Sprite::Update(float _dt)
{
	m_Sprite.setPosition(m_Owner->GetComponent<C_Transform>()->GetPosition());
}

void C_Sprite::Draw(sf::RenderWindow& _window)
{
	_window.draw(m_Sprite);
}

void C_Sprite::SetTexture(const sf::Texture& _texture)
{
	m_Sprite.setTexture(_texture);
}

void C_Sprite::SetTextureRect(const sf::IntRect& _rect)
{
	m_Sprite.setTextureRect(_rect);
}

void C_Sprite::SetScale(const sf::Vector2f& _scale)
{
	m_Sprite.setScale(_scale);
}

void C_Sprite::SetLayerIndex(unsigned int _layer)
{
	m_LayerIndex = _layer;
}

unsigned int C_Sprite::GetLayerIndex() const
{
	return m_LayerIndex;
}

void C_Sprite::SetDrawableType(ETag _type)
{
	m_DrawableType = _type;
}

ETag C_Sprite::GetDrawableType() const
{
		return m_DrawableType;
}
