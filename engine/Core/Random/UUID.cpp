#include "engine/pch.hpp"

#include "UUID.hpp"
#include <random>

namespace Random
{

	static std::random_device s_RandomDevice;
	static std::mt19937_64 s_RandomEngine(s_RandomDevice());
	static std::uniform_int_distribution<uint64_t> s_Distribution;

	UUID::UUID()
		: m_UUID(s_Distribution(s_RandomEngine))
	{
	}

	UUID::UUID(uint64_t _uuid_int64) 
		: m_UUID(_uuid_int64)
	{
	}


	Random::UUID& UUID::operator=(uint64_t _uuid_int64)
	{
		m_UUID = _uuid_int64;
		return *this;
	}


	UUID::UUID(const UUID& _other)
		: m_UUID(_other.GetUUID())
	{

	}

	UUID::UUID(UUID&& _other) noexcept
		: m_UUID(std::exchange(_other.m_UUID, Random::EMPTY_UUID))
	{

	}


	Random::UUID& UUID::operator=(const UUID& _other)
	{
		return *this = UUID(_other);
	}

	Random::UUID& UUID::operator=(UUID&& _other) noexcept
	{ 
		std::swap(m_UUID, _other.m_UUID);
		return *this;
	}

}