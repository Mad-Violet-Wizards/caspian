#include "engine/pch.hpp"

bool Tools_Impl::utils::FilterDigitOnly(const std::string& _str)
{
	return std::all_of(_str.begin(), _str.end(), [](char c) { return std::isdigit(c); }) && !_str.empty();
}
