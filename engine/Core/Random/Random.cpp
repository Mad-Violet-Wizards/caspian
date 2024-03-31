#include "engine/pch.hpp"

#include "Random.hpp"
#include <random>

std::string Random::GenerateRandomString(unsigned int _len)
{
	// https://stackoverflow.com/questions/440133/how-do-i-create-a-random-alpha-numeric-string-in-c
	auto randchar = []() -> char
	{
		static constexpr auto charset = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
		const size_t max_index = strlen(charset) - 1;
		thread_local auto rng = std::mt19937{ std::random_device{}() };
		static std::uniform_int_distribution<> uniform_dist(0, max_index);
		return charset[uniform_dist(rng)];
	};

	std::string str(_len, 0);
	std::generate_n(str.begin(), _len, randchar);
	return str;
}
