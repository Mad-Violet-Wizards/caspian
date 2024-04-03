#pragma once

#include "engine/Core/IComponent.hpp"

class C_Sprite : public IComponent
{
	public:

		C_Sprite(GameObject* _owner);

		virtual void Update(float _dt) override;
		virtual void LateUpdate([[maybe_unused]] float _dt) override {};
		void Draw(sf::RenderWindow& _window);

		void SetTexture(const sf::Texture& texture);
		void SetTextureRect(const sf::IntRect& rect);
		void SetScale(const sf::Vector2f& scale);

		void SetLayer(unsigned int _layer);
		unsigned int GetLayer() const;

	private:

		sf::Sprite m_Sprite;

		unsigned int m_Layer = -1;
};