#include "engine/pch.hpp"
#include "engine/Core/Animations.hpp"
#include "AnimationsSerializable.hpp"

Serializable::JSON::AnimationFrameInfo::AnimationFrameInfo(float _duration, int _x, int _y, int _width, int _height)
	: m_Duration(_duration)
	, m_X(_x)
	, m_Y(_y)
	, m_Width(_width)
	, m_Height(_height)
{

}

Serializable::JSON::AnimationInfo::AnimationInfo(const std::string& _name, const std::string& _texture_key, EAnimationType _anim_type, const std::vector<AnimationFrameInfo>& _frames)
	: m_Name(_name)
	, m_TextureName(_texture_key)
	, m_AnimType(_anim_type)
	, m_Frames(_frames)
{

}

