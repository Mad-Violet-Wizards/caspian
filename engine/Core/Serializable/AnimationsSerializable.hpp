#pragma once

#include <cereal/archives/portable_binary.hpp>
#include <cereal/archives/json.hpp>

#include "Serializable.hpp"

enum class EAnimationType;

namespace Serializable
{
	namespace JSON
	{
		class AnimationFrameInfo : public ISerializable::JSON
		{
			public:

				AnimationFrameInfo() = default;
				AnimationFrameInfo(float _duration, int _x, int _y, int _width, int _height);

				~AnimationFrameInfo() = default;

				void dummy() override {};

				template<class Archive>
				void serialize(Archive& ar)
				{
						ar(cereal::make_nvp("Duration", m_Duration),
						cereal::make_nvp("X", m_X),
						cereal::make_nvp("Y", m_Y),
						cereal::make_nvp("Width", m_Width),
						cereal::make_nvp("Height", m_Height));
				}

			public:

				float m_Duration;
				int m_X;
				int m_Y;
				int m_Width;
				int m_Height;
		};

		class AnimationInfo : public ISerializable::JSON
		{
			public:

				AnimationInfo() = default;
				AnimationInfo(const std::string& _name, const std::string& _texture_key, EAnimationType _anim_type, const std::vector<AnimationFrameInfo>& _frames);

				~AnimationInfo() = default;

				void dummy() override {};

				template<class Archive>
				void serialize(Archive& ar)
				{
					ar(cereal::make_nvp("Name", m_Name),
						cereal::make_nvp("TextureName", m_TextureName),
						cereal::make_nvp("AnimType", m_AnimType),
						cereal::make_nvp("Frames", m_Frames));
				}

			public:

				std::string m_Name;
				std::string m_TextureName;
				EAnimationType m_AnimType;
				std::vector<AnimationFrameInfo> m_Frames;
		};
	}
}

using namespace Serializable::JSON;
using namespace ISerializable;

CEREAL_REGISTER_TYPE(AnimationFrameInfo);
CEREAL_REGISTER_POLYMORPHIC_RELATION(JSON, AnimationFrameInfo);

CEREAL_REGISTER_TYPE(AnimationInfo);
CEREAL_REGISTER_POLYMORPHIC_RELATION(JSON, AnimationInfo);