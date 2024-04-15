#pragma once

#include "engine/Core/Bitmask.hpp"

class KeyboardInputController
{
public:

	KeyboardInputController() = default;
	~KeyboardInputController() = default;

	void Update([[maybe_unused]] float _dt);

	void OnKeyPressed(sf::Keyboard::Key _key);
	void OnKeyReleased(sf::Keyboard::Key _key);

	bool IsKeyPressed(sf::Keyboard::Key _key);
	bool IsKeyDown(sf::Keyboard::Key _key);
	bool IsKeyUp(sf::Keyboard::Key _key);

private:

	Bitmask<uint64_t> m_keyStatePrev;
	Bitmask<uint64_t> m_keyState;

};