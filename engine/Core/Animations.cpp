#include "engine/pch.hpp"
#include "Animations.hpp"

#include "engine/Core/Serializable/AnimationsSerializable.hpp"

AnimationFrame::AnimationFrame(float _duration, const sf::IntRect& _rect)
	: m_Duration(_duration)
	, m_Rect(_rect)
{

}

Animation::Animation(const std::string& _name, const std::string& _texture_key, EAnimationType _type)
	: m_Name(_name)
	, m_TextureKey(_texture_key)
	, m_Type(_type)
{

}

Animation::Animation()
	: m_Name("Unknown")
	, m_TextureKey("")
	, m_Type(EAnimationType::Unknown)
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

void Animation::PerformSave()
{
	auto& main_instance = ApplicationSingleton::Instance();

	std::filesystem::path path = "animations";
	path /= m_Name + fs::IFile::TypeToStringExt(fs::IFile::EType::JSON);

	fs::IFileSystem* data_fs = main_instance.GetEngineController().GetFilesystemManager()->Get("data");

	bool json_ok = false;

	if (!data_fs->FileExists(path.string()))
	{
		json_ok = data_fs->CreateFile(path.string(), fs::IFile::EType::JSON);
	}
	else
	{
		json_ok = true;
	}

	if (json_ok)
	{
		if (std::shared_ptr<fs::IFile> anim_file = data_fs->OpenFile(path.string(), fs::io::OpenMode::Out))
		{

			std::vector<AnimationFrameInfo> frames_info_data;

			for (const AnimationFrame& frame : m_Frames)
			{
				AnimationFrameInfo frame_data(frame.m_Duration, frame.m_Rect.left, frame.m_Rect.top, frame.m_Rect.width, frame.m_Rect.height);
				frames_info_data.push_back(frame_data);
			}

			std::shared_ptr<ISerializable::JSON> anim_data = std::make_shared<Serializable::JSON::AnimationInfo>(m_Name, m_TextureKey, m_Type, frames_info_data);
			
			anim_file->Seek(0, fs::io::Origin::Begin);
			anim_file->SerializeJson(anim_data);
			data_fs->CloseFile(anim_file);
		}
	}
}

bool AnimationsController::CreateNewAnimation(const std::string& _name, const std::string& _texture_key, EAnimationType _type, const std::vector<AnimationFrame>& _frames)
{
	if (m_Animations.find(_name) != m_Animations.end())
	{
		return false;
	}

	Animation animation(_name, _texture_key, _type);
	for (const auto& frame : _frames)
	{
		animation.AddFrame(frame);
	}

	animation.PerformSave();

	m_Animations[_name] = animation;
	return true;
}

void AnimationsController::PushAnimationDataFromLoading(std::shared_ptr<Serializable::JSON::AnimationInfo>& _anim_info)
{
	if (m_Animations.find(_anim_info->m_Name) != m_Animations.end())
	{
		return;
	}

	Animation animation(_anim_info->m_Name, _anim_info->m_TextureName, _anim_info->m_AnimType);

	for (const auto& frame : _anim_info->m_Frames)
	{
		animation.AddFrame(frame.m_Duration, sf::IntRect(frame.m_X, frame.m_Y, frame.m_Width, frame.m_Height));
	}

	m_Animations[_anim_info->m_Name] = animation;
}
