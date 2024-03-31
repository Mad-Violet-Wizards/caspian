#pragma once

namespace Math
{
	template<typename T>
	T is_pow_of_2(T _num)
	{
		return (_num != 0) && (_num & (_num - 1)) == 0;
	}

	template<typename T>
	T round_up(T _value_to_round, T multiple_factor)
	{
		if (is_pow_of_2<T>(_value_to_round))
		{
			return (_value_to_round + multiple_factor - 1) & ~(multiple_factor);
		}
		else
		{
			return ((_value_to_round + multiple_factor - 1) / multiple_factor) * multiple_factor;
		}
	}
}