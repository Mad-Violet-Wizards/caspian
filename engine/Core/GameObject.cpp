#include "engine/pch.hpp"

#include "GameObject.hpp"

GameObject::GameObject() 
	: m_queuedForRemoval(false) 
{
}

GameObject::~GameObject()
{
	m_components.clear();
}

void GameObject::Awake()
{
	for (const auto& c : m_components)
		c->Awake();
}

void GameObject::Start()
{
	for (const auto& c : m_components)
		c->Start();
}

void GameObject::Update(float deltaTime)
{
	for (const auto& c : m_components)
		c->Update(deltaTime);
}

void GameObject::LateUpdate(float deltaTime)
{
	for (const auto& c : m_components)
		c->LateUpdate(deltaTime);
}

void GameObject::Draw(Window& window)
{
	for (const auto& c : m_components)
		c->Draw(window);
}

bool GameObject::QueuedForRemoval() const
{
	return m_queuedForRemoval;
}

void GameObject::QueueForRemoval()
{
	m_queuedForRemoval = true;
}