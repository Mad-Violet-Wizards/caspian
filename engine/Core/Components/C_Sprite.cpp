#include "engine/pch.hpp"

#include "C_Sprite.hpp"
#include "C_Transform.hpp"

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

void C_Sprite::SetLayer(unsigned int _layer)
{
	m_Layer = _layer;
}

unsigned int C_Sprite::GetLayer() const
{
	return m_Layer;
}
