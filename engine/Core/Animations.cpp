#include "engine/pch.hpp"
#include "Animations.hpp"

AnimationFrame::AnimationFrame(float _duration, const sf::IntRect& _rect)
	: m_Duration(_duration)
	, m_Rect(_rect)
{

}

Animation::Animation(const std::string& _name, EAnimationType _type)
	: m_Name(_name)
	, m_Type(_type)
{

}

void Animation::AddFrame(const AnimationFrame& _frame)
{
	m_Frames.push_back(_frame);
}

void Animation::AddFrame(float _duration, const sf::IntRect& _rect)
{
	m_Frames.push_back({ _duration, _rect });
}

bool AnimationsStorage::CreateNewAnimation(const std::string& _name, EAnimationType _type, const std::vector<AnimationFrame>& _frames)
{
	if (m_Animations.find(_name) != m_Animations.end())
	{
		return false;
	}

	Animation animation(_name, _type);
	for (const auto& frame : _frames)
	{
		animation.AddFrame(frame);
	}

	m_Animations[_name] = animation;
	return true;

}
