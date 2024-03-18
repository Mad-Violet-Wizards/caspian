#pragma once

namespace Random
{
	class UUID
	{
		public:
			
			UUID();
			UUID(uint64_t _uuid);
			UUID(const UUID&) = default;

			~UUID() = default;

			operator uint64_t() const { return m_UUID; }

			uint64_t GetUUID() const { return m_UUID;  }

	private:

				uint64_t m_UUID;

	};

	static UUID EMPTY_UUID = 0;
}

namespace std
{
	template<typename T>
	struct hash;

	template<>
	struct hash<Random::UUID>
	{
		std::size_t operator()(const Random::UUID& _uuid) const
		{
			return static_cast<uint64_t>(_uuid);
		}
	};
}