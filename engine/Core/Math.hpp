#pragma once

namespace Math
{
	template<typename T>
	T tile_round_up(T _value_to_round, T _multiple_factor)
	{
		// 0 - 32 -> 0
		// 33 - 64 -> 32
		// 65 - 96 -> 64
		// and so on...

		if (_value_to_round <= _multiple_factor)
			return 0;

		return (floor((_value_to_round + _multiple_factor - 1) / _multiple_factor) * _multiple_factor) - _multiple_factor;
	
	}
}