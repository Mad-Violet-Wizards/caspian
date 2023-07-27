#include "engine/pch.hpp"

#include "tests/catch.hpp"

namespace moc
{
	class Toggle
	{
		public:

			Toggle() = default;
			~Toggle() = default;

			void Switch() { m_state = !m_state; }

		public:

			bool m_state : 1 = false;
	};
};

TEST_CASE("Event Listener event type", "[EventListenerTests]")
{
	EventListener listener;
	listener.NotifyOn(sf::Event::Closed);

	REQUIRE(listener.GetEventType() == sf::Event::Closed);
}

TEST_CASE("Event Dispatcher managment of EventListeners - Add & Remove", "[EventListenerTests]")
{
	EventListener listenerA;
	EventListener listenerB;

	EventDispatcher dispatcher;
	EventDispatcherTestsWrapper dispatcher_helper(&dispatcher);

	SECTION("EventListener add to the list")
	{
		dispatcher.AddObserver(&listenerA);
		dispatcher.AddObserver(&listenerB);

		REQUIRE(dispatcher_helper.GetObserversVecSize() == 2);
		REQUIRE(dispatcher_helper.IsListenerAdded(&listenerA));
		REQUIRE(dispatcher_helper.IsListenerAdded(&listenerB));
	}

	SECTION("EventListener remove from the list")
	{
		dispatcher.AddObserver(&listenerA);
		dispatcher.AddObserver(&listenerB);
		dispatcher.RemoveObserver(&listenerA);

		REQUIRE(dispatcher_helper.GetObserversVecSize() == 1);
		REQUIRE(dispatcher_helper.IsListenerAdded(&listenerB));

		REQUIRE_FALSE(dispatcher_helper.IsListenerAdded(&listenerA));
	}
}

TEST_CASE("Events are dispatched correctly to listeners.", "[EventListener]")
{
	EventListener listenerA;
	listenerA.NotifyOn(sf::Event::Closed);

	moc::Toggle listener_a_toggle;
	listenerA.SetCallback([&listener_a_toggle](const sf::Event& event)
		{
			listener_a_toggle.Switch();
		});

	EventListener listenerB;
	listenerB.NotifyOn(sf::Event::Resized);
	moc::Toggle listener_b_toggle;
	listenerB.SetCallback([&listener_b_toggle](const sf::Event& event) {
			listener_b_toggle.Switch();
		});


	EventDispatcher dispatcher;
	dispatcher.AddObserver(&listenerA);
	dispatcher.AddObserver(&listenerB);
	
	sf::Event example_event;
	example_event.type = sf::Event::Closed;

	dispatcher.ProcessEvent(example_event);

	REQUIRE(listener_a_toggle.m_state == true);
	REQUIRE(listener_b_toggle.m_state == false);
}
