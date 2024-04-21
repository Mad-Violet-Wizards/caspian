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

bool GameObject::QueuedForRemoval() const
{
	return m_queuedForRemoval;
}

void GameObject::QueueForRemoval()
{
	m_queuedForRemoval = true;
}

void GameObject::OnCollisionEnter()
{

}

void GameObject::OnCollisionStay()
{

}

void GameObject::OnCollisionExit()
{

}
