#include "engine/pch.hpp"
#include "KeyboardInputController.hpp"

void KeyboardInputController::Update(float _dt)
{
	m_keyStatePrev = m_keyState;
}

void KeyboardInputController::OnKeyPressed(ESupportedKey _key)
{
	m_keyState.SetBit((unsigned int)_key, true);
}
	
void KeyboardInputController::OnKeyReleased(ESupportedKey _key)
{
	m_keyState.ClearBit((unsigned int)_key);
}

bool KeyboardInputController::IsKeyPressed(ESupportedKey _key)
{
	return m_keyState.GetBit((unsigned int)_key);
}

bool KeyboardInputController::IsKeyDown(ESupportedKey _key)
{
	bool this_frame = m_keyState.GetBit((unsigned int)_key);
	bool last_frame = m_keyStatePrev.GetBit((unsigned int)_key);

	return this_frame && !last_frame;
}

bool KeyboardInputController::IsKeyUp(ESupportedKey _key)
{
	bool this_frame = m_keyState.GetBit((unsigned int)_key);
	bool last_frame = m_keyStatePrev.GetBit((unsigned int)_key);

	return !this_frame && last_frame;
}

ESupportedKey KeyboardInputController::GetSupportedKey(sf::Keyboard::Key _key) const
{
	switch (_key)
	{
	case sf::Keyboard::Grave:
		return ESupportedKey::Grave;
	case sf::Keyboard::W:
		return ESupportedKey::W;
	case sf::Keyboard::A:
		return ESupportedKey::A;
	case sf::Keyboard::S:
		return ESupportedKey::S;
	case sf::Keyboard::D:
		return ESupportedKey::D;
	}
}

bool KeyboardInputController::IsKeySupported(sf::Keyboard::Key _key) const
{
	switch (_key)
	{
		case sf::Keyboard::Grave:
		case sf::Keyboard::W:
		case sf::Keyboard::A:
		case sf::Keyboard::S:
		case sf::Keyboard::D:
		return true;
	}

	return false;
}
