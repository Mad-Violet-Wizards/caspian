#pragma once

#include "engine/Core/Bitmask.hpp"

enum class ESupportedKey : uint64_t
{
	Grave = 0,
	W = 1,
	A,
	S,
	D = 4
};

class KeyboardInputController
{
public:

	KeyboardInputController() = default;
	~KeyboardInputController() = default;

	void Update([[maybe_unused]] float _dt);

	void OnKeyPressed(ESupportedKey _key);
	void OnKeyReleased(ESupportedKey _key);

	bool IsKeyPressed(ESupportedKey _key);
	bool IsKeyDown(ESupportedKey _key);
	bool IsKeyUp(ESupportedKey _key);

	ESupportedKey GetSupportedKey(sf::Keyboard::Key _key) const;
	bool IsKeySupported(sf::Keyboard::Key _key) const;

private:

	Bitmask<uint64_t> m_keyStatePrev;
	Bitmask<uint64_t> m_keyState;

};