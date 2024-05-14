#pragma once

class Window;

#include "IScene.hpp"

namespace Scenes
{
	enum class ESceneID : char
	{
		_Start = -1,
		InitialLoading = 0,
		MainMenu,
		Game,
		#if defined(DEBUG)
		Edit,
		#endif
		_End
	};

	static const char* ToString(ESceneID _id)
	{
		#define ENUM_TO_STR(x) case ESceneID::x: return #x;
		switch (_id)
		{
			ENUM_TO_STR(InitialLoading)
			ENUM_TO_STR(MainMenu)
			ENUM_TO_STR(Game)
#if defined(DEBUG)
			ENUM_TO_STR(Edit)
#endif
		}
	}

	constexpr std::initializer_list<ESceneID> AllScenes()
	{
#if defined(DEBUG)
		return { ESceneID::InitialLoading, ESceneID::MainMenu, ESceneID::Game, ESceneID::Edit };
#else
		return { ESceneID::InitialLoading, ESceneID::MainMenu, ESceneID::Game };
#endif
	}

	class StateMachine
	{

	public:

		StateMachine() = default;
		~StateMachine() = default;

		void Update(float deltaTime);
		void LateUpdate(float deltaTime);

		bool Add(ESceneID id, std::shared_ptr<IScene> scene);
		bool SwitchTo(ESceneID id);
		bool Remove(ESceneID id);

	private:

		std::unordered_map<ESceneID, std::shared_ptr<IScene>> m_scenes;
		std::shared_ptr<IScene> m_currentScene;

	};
};