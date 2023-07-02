#pragma once

class Window;

#include "IScene.hpp"

namespace Scenes
{
	enum class ESceneID : char
	{
		Loading = 0,
		MainMenu,
		Game
	};

	class StateMachine
	{

	public:

		StateMachine() = default;
		~StateMachine() = default;

		void Update(float deltaTime);
		void LateUpdate(float deltaTime);
		void Draw(Window& window);

		bool Add(ESceneID id, std::shared_ptr<IScene> scene);
		bool SwitchTo(ESceneID id);
		bool Remove(ESceneID id);

	private:

		std::unordered_map<ESceneID, std::shared_ptr<IScene>> m_scenes;
		std::shared_ptr<IScene> m_currentScene;

	};
};