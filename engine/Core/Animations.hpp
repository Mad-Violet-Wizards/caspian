#pragma once

#include "engine/Core/Serializable/AnimationsSerializable.hpp"

///////////////////////////////////////////////////////
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

	Animation();
	Animation(const std::string& _name, const std::string& _texture_key, EAnimationType _type);
	~Animation() = default;

	void AddFrame(const AnimationFrame& _frame);
	void AddFrame(float _duration, const sf::IntRect& _rect);

	const std::string& GetName() const { return m_Name; }
	const std::string& GetTextureKey() const { return m_TextureKey; }
	EAnimationType GetType() const { return m_Type; }

	int GetFramesSize() const { return m_Frames.size(); }
	const std::vector<AnimationFrame>& GetFrames() const { return m_Frames; }
	const AnimationFrame& GetFrame(int _index) const { return m_Frames[_index]; }

	void PerformSave();

private:

	std::string m_Name;
	std::string m_TextureKey;
	EAnimationType m_Type;
	std::vector<AnimationFrame> m_Frames;
};


/////////////////////////////////////////////////////////
class AnimationsController
{
	public:

		AnimationsController();
		~AnimationsController() = default;

		void Update(float _dt);

		bool CreateNewAnimation(const std::string& _name, const std::string& _texture_key, EAnimationType _type, const std::vector<AnimationFrame>& _frames);
		void PushAnimationDataFromLoading(std::shared_ptr<Serializable::JSON::AnimationInfo>& _anim_info);

		void PlayAnimation(const std::string& _name);
		void StopAnimation();

		const Animation* GetCurrentAnimation() const;
		const AnimationFrame* GetCurrentAnimationFrame() const;

	private:

		void OnAnimationEnd();

	private:

		std::unordered_map<std::string, Animation> m_Animations;

		bool m_OneShotBackwards;

		const std::string* m_CurrentAnimationName;
		float m_CurrentFrameTime;
		int m_CurrentFrameIndex;
};
