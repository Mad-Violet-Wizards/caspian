#pragma once

#include <cereal/types/polymorphic.hpp>
#include <cereal/archives/json.hpp>

#define SERIALIZE_BINARY(...) \
	void dummy() const override {} \
	\
	template<typename Archive> \
	void serialize(Archive& ar) \
	{ \
		ar(__VA_ARGS__); \
	}

#define SERIALIZE_JSON(...) \
	void dummy() const override {} \
	\
	template<typename Archive> \
	void serialize(Archive& ar) \
	{ \
		ar(CEREAL_NVP(__VA_ARGS__)); \
	}

#define REGISTER_JSON_SERIALIZABLE(x) \
	CEREAL_REGISTER_TYPE(x); \
	CEREAL_REGISTER_POLYMORPHIC_RELATION(ISerializable::JSON, x)

#define REGISTER_BINARY_SERIALIZABLE(x) \
	CEREAL_REGISTER_TYPE(x); \
	CEREAL_REGISTER_POLYMORPHIC_RELATION(ISerializable::Binary, x)

namespace ISerializable
{
	class Binary
	{
		public:
		
			virtual ~Binary() = default;

			virtual void dummy() = 0;


	};

	class JSON
	{
		public:
		
			virtual ~JSON() = default;

			virtual void dummy() = 0;

	};
}
