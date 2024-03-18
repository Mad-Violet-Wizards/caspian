#pragma once

class Window;

namespace Scenes
{
	class IScene
	{

	public:

		IScene() = default;
		~IScene() = default;

		virtual void OnCreate() = 0;
		virtual void OnDestroy() = 0;

		virtual void OnActivate() = 0;
		virtual void OnDeactivate() = 0;

		virtual void Update(float deltaTime) = 0;
		virtual void LateUpdate(float deltaTime) = 0;

	};
};