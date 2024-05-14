#pragma once

/////////////////////////////////////////////////////////
enum class EAnimationType
{
	Unknown,
	Loop,
	OneShot,
	PingPong
};

inline static const char* ToString(EAnimationType _type)
{
	switch (_type)
	{
		case EAnimationType::Loop: return "Loop";
		case EAnimationType::OneShot: return "OneShot";
		case EAnimationType::PingPong: return "PingPong";
		default: return "Unknown";
	}
}

/////////////////////////////////////////////////////////
class AnimationFrame
{
public:

	AnimationFrame(float _duration, const sf::IntRect& _rect);

	float m_Duration;
	sf::IntRect m_Rect;
};

/////////////////////////////////////////////////////////
class Animation
{
public:

	Animation(const std::string& _name, EAnimationType _type);
	~Animation() = default;

	void AddFrame(const AnimationFrame& _frame);
	void AddFrame(float _duration, const sf::IntRect& _rect);

	const std::string& GetName() const { return m_Name; }
	EAnimationType GetType() const { return m_Type; }
	const std::vector<AnimationFrame>& GetFrames() const { return m_Frames; }

private:

	std::string m_Name;
	EAnimationType m_Type;
	std::vector<AnimationFrame> m_Frames;
};


/////////////////////////////////////////////////////////
class AnimationsStorage
{
	public:

		AnimationsStorage() = default;
		~AnimationsStorage() = default;

		bool CreateNewAnimation(const std::string& _name, EAnimationType _type, const std::vector<AnimationFrame>& _frames);

	private:

		std::unordered_map<std::string, Animation> m_Animations;
};
