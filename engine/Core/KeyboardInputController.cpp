#include "engine/pch.hpp"
#include "KeyboardInputController.hpp"

void KeyboardInputController::Update(float _dt)
{
	m_keyStatePrev = m_keyState;
}

void KeyboardInputController::OnKeyPressed(sf::Keyboard::Key _key)
{
	m_keyState.SetBit((unsigned int)_key, true);
}

void KeyboardInputController::OnKeyReleased(sf::Keyboard::Key _key)
{
	m_keyState.ClearBit((unsigned int)_key);
}

bool KeyboardInputController::IsKeyPressed(sf::Keyboard::Key _key)
{
	return m_keyState.GetBit((unsigned int)_key);
}

bool KeyboardInputController::IsKeyDown(sf::Keyboard::Key _key)
{
	return m_keyState.GetBit((unsigned int)_key) && !m_keyStatePrev.GetBit((unsigned int)_key);
}

bool KeyboardInputController::IsKeyUp(sf::Keyboard::Key _key)
{
	return !m_keyState.GetBit((unsigned int)_key) && m_keyStatePrev.GetBit((unsigned int)_key);
}
