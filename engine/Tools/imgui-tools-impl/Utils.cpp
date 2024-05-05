#include "engine/pch.hpp"

bool Tools_Impl::utils::FilterDigitOnly(const std::string& _str)
{
	const bool empty = _str.empty();

	if (empty)
		return false;

	return std::all_of(_str.begin(), _str.end(), [](char c) { return std::isdigit(c); });
}
