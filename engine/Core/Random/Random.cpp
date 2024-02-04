#include "engine/pch.hpp"

#include "Random.hpp"

std::string Random::GenerateRandomString(unsigned int _len)
{
	// https://stackoverflow.com/questions/440133/how-do-i-create-a-random-alpha-numeric-string-in-c
	auto randchar = []() -> char
	{
		const char charset[] =
			"0123456789"
			"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
			"abcdefghijklmnopqrstuvwxyz";
		const size_t max_index = (sizeof(charset) - 1);
		return charset[std::rand() % max_index];
	};

	std::string str(_len, 0);
	std::generate_n(str.begin(), _len, randchar);
	return str;
}
