#pragma once

#include "engine/Core/IComponent.hpp"
#include "engine/Core/Components/C_Tags.hpp"

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

		void SetLayerIndex(unsigned int _layer);
		unsigned int GetLayerIndex() const;

		void SetDrawableType(ETag _type);
		ETag GetDrawableType() const;

	private:

		sf::Sprite m_Sprite;

		ETag m_DrawableType = ETag::None;
		unsigned int m_LayerIndex = -1;
};