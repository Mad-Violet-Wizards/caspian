#include "engine/pch.hpp"

#include "SceneStateMachine.hpp"

using namespace Scenes;

void StateMachine::Update(float deltaTime)
{
	if (m_currentScene)
		m_currentScene->Update(deltaTime);
}

void StateMachine::LateUpdate(float deltaTime)
{
	if (m_currentScene)
		m_currentScene->LateUpdate(deltaTime);
}

void StateMachine::Draw(Window& window)
{
	if (m_currentScene)
		m_currentScene->Draw(window);
}

bool StateMachine::Add(ESceneID id, std::shared_ptr<IScene> scene)
{
	auto it = m_scenes.try_emplace(id, scene);

	scene->OnCreate();

	return it.second;
}

bool StateMachine::SwitchTo(ESceneID id)
{
	auto it = m_scenes.find(id);

	if (it == m_scenes.cend())
		return false;

	auto switch_to_scene_id = it->first;
	auto switch_to_scene = it->second;

	if (m_currentScene)
		m_currentScene->OnDeactivate();

	m_currentScene = switch_to_scene;
	m_currentScene->OnActivate();

	return true;
}

bool StateMachine::Remove(ESceneID id)
{
	auto it = m_scenes.find(id);

	if (it == m_scenes.cend())
		return false;

	auto remove_scene_id = it->first;
	auto remove_scene = it->second;

	if (m_currentScene == remove_scene)
		m_currentScene = nullptr;

	remove_scene->OnDestroy();
	m_scenes.erase(it);

	return true;
}