#pragma once

#include "IComponent.hpp"
#include "Window.hpp"

class GameObject
{
	public:

		GameObject();

		~GameObject();

		void Awake();
		void Start();

		void Update(float deltaTime);
		void LateUpdate(float deltaTime);
		void Draw(Window& window);

		bool QueuedForRemoval() const;
		void QueueForRemoval();

		template<typename T, typename ...Args>
		[[nodiscard]] std::shared_ptr<T> AddComponent(Args &&...args)
		{
			static_assert(std::is_base_of<IComponent, T>::value, "T must derive from IComponent class.");

			// Do not allow duplicates.
			for (const auto& existingComponent : m_components)
				if (std::shared_ptr<T> component = std::dynamic_pointer_cast<T>(existingComponent)) [[unlikely]]
					return component;

			std::shared_ptr<T> newComponent = std::make_shared<T>(this, std::forward<Args>(args)...);
			m_components.push_back(newComponent);
		}

		template<typename T>
		[[nodiscard]] std::shared_ptr<T> GetComponent()
		{
			for (const auto& existingComponent : m_components)
				if (std::dynamic_pointer_cast<T>(existingComponent))
					return existingComponent;

			return nullptr;
		}

		template<typename T>
		[[nodiscard]] bool HasComponent() const
		{
			for (const auto& existingComponent : m_components)
			{
				if (std::dynamic_pointer_cast<T>(existingComponent))
					return true;
			}

			return false;
		}

	private:

		bool m_queuedForRemoval : 1;

		std::vector<std::shared_ptr<IComponent>> m_components;
};