#include "engine/pch.hpp"

#include "../catch.hpp"

#include "engine/Scenes/IScene.hpp"
#include "engine/Scenes/StateMachine.hpp"

namespace moc
{
	using namespace Scenes;

	class SceneA : public IScene
	{
		public:

			SceneA()
				: m_wasCreated(false)
				, m_wasDestroyed(false)
				, m_wasActivated(false)
				, m_wasDeactivated(false)
			{}

			void OnCreate() override { m_wasCreated = true; }
			void OnDestroy() override{ m_wasDestroyed = true; }

			void OnActivate() override { m_wasActivated = true; }
			void OnDeactivate() override { m_wasDeactivated = true; }

			void Update(float deltaTime) override {}
			void LateUpdate(float deltaTime) override {}
			void Draw(Window& window) override {}

			bool m_wasCreated : 1;
			bool m_wasDestroyed : 1;
			bool m_wasActivated : 1;
			bool m_wasDeactivated : 1;
	};

	class SceneB : public IScene
	{
		public:

			SceneB()
				: m_wasCreated(false)
				, m_wasDestroyed(false)
				, m_wasActivated(false)
				, m_wasDeactivated(false)
			{}

			void OnCreate() override { m_wasCreated = true; }
			void OnDestroy() override { m_wasDestroyed = true; }

			void OnActivate() override { m_wasActivated = true; }
			void OnDeactivate() override { m_wasDeactivated = true; }

			void Update(float deltaTime) override {}
			void LateUpdate(float deltaTime) override {}
			void Draw(Window& window) override {}

			bool m_wasCreated : 1;
			bool m_wasDestroyed : 1;
			bool m_wasActivated : 1;
			bool m_wasDeactivated : 1;
	};
};

TEST_CASE("ScenesTests", "[EmptySceneStateMachine]")
{
	Scenes::StateMachine state_machine;

	REQUIRE_NOTHROW(state_machine.Update(0.f));
	REQUIRE_NOTHROW(state_machine.LateUpdate(0.f));
 
	// There's no way to ommit C2280 and create null-ref.
	// state_machine.Draw();
}

TEST_CASE("SceneTests", "[AddingScenesToStateMachine]")
{
	Scenes::StateMachine state_machine;

	auto scene1 = std::make_shared<moc::SceneA>();
	auto scene2 = std::make_shared<moc::SceneB>();

	const bool ok = state_machine.Add(Scenes::ESceneID::Game, scene1);
	const bool ok2 = state_machine.Add(Scenes::ESceneID::MainMenu, scene2);

	REQUIRE(ok == true);
	REQUIRE(ok2 == true);
}

TEST_CASE("ScenesTests", "[DuplicateScenesAddedToStateMachine]")
{
	Scenes::StateMachine state_machine;

	auto scene1 = std::make_shared<moc::SceneA>();
	auto scene2 = std::make_shared<moc::SceneB>();

	const bool ok = state_machine.Add(Scenes::ESceneID::Game, scene1);
	const bool ok2 = state_machine.Add(Scenes::ESceneID::Game, scene2);

	REQUIRE(ok == true);
	REQUIRE(ok2 == false);
}

TEST_CASE("ScenesTests", "[SwitchBetweenScenes]")
{
	Scenes::StateMachine state_machine;

	auto scene1 = std::make_shared<moc::SceneA>();
	auto scene2 = std::make_shared<moc::SceneB>();

	state_machine.Add(Scenes::ESceneID::MainMenu, scene1);
	state_machine.Add(Scenes::ESceneID::Game, scene2);

	const bool ok = state_machine.SwitchTo(Scenes::ESceneID::MainMenu);
	const bool ok2 = state_machine.SwitchTo(Scenes::ESceneID::Game);

	REQUIRE(ok == true);
	REQUIRE(ok2 == true);
}

TEST_CASE("ScenesTests", "[SwitchToNonExsitingScene]")
{
	Scenes::StateMachine state_machine;

	const bool ok = state_machine.SwitchTo(Scenes::ESceneID::MainMenu);
	
	REQUIRE(ok == false);
}

TEST_CASE("ScenesTests", "[RemoveScene]")
{
	Scenes::StateMachine state_machine;

	auto scene1 = std::make_shared<moc::SceneA>();
	auto scene2 = std::make_shared<moc::SceneB>();

	state_machine.Add(Scenes::ESceneID::MainMenu, scene1);
	state_machine.Add(Scenes::ESceneID::Game, scene2);

	state_machine.SwitchTo(Scenes::ESceneID::MainMenu);

	const bool ok = state_machine.Remove(Scenes::ESceneID::MainMenu);
	const bool ok2 = state_machine.Remove(Scenes::ESceneID::Game);
	
	REQUIRE(ok == true);
	REQUIRE(ok2 == true);
}

TEST_CASE("ScenesTests", "[RemoveNonExistingScene]")
{
	Scenes::StateMachine state_machine;

	const bool ok = state_machine.Remove(Scenes::ESceneID::MainMenu);

	REQUIRE(ok == false);
}