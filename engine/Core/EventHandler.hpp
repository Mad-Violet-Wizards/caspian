#pragma once

#include <functional>

////////////////////////////////////////////////////////////////////////////////////////////
class Observer
{
  public:

    Observer() = default;
    virtual ~Observer() = default;

    void NotifyOn(sf::Event::EventType eventType) { m_eventType = eventType; }
    virtual void OnNotify(const sf::Event& event) = 0;

    sf::Event::EventType GetEventType() const { return m_eventType; }

  protected:

    sf::Event::EventType m_eventType;
};

////////////////////////////////////////////////////////////////////////////////////////////
class EventListener : public Observer
{

public:

  EventListener() = default;
  ~EventListener() = default;

  void OnNotify(const sf::Event& event) override
  {
    if (m_callback)
      m_callback(event);
  }

  void SetCallback(std::function<void(const sf::Event&)> callback) { m_callback = callback; }

private:

  std::function<void(const sf::Event&)> m_callback;
};

////////////////////////////////////////////////////////////////////////////////////////////
class EventDispatcher
{

  public:

    EventDispatcher() = default;
    ~EventDispatcher() = default;

    void AddObserver(Observer* observer) { m_observers.push_back(observer); }
    void RemoveObserver(Observer* observer) { m_observers.erase(std::remove(m_observers.begin(), m_observers.end(), observer), m_observers.end()); }
    void ProcessEvent(const sf::Event& event)
    {
      if (m_observers.size() == 0)
        return;

      for (auto& observer : m_observers)
      {
        if (observer->GetEventType() == event.type)
          observer->OnNotify(event);
      }
    }

  private:

    std::vector<Observer*> m_observers;

  public:
    
    #if (defined TESTS)
      friend class EventDispatcherTestsWrapper;
    #endif
};

#if (defined TESTS)
  class EventDispatcherTestsWrapper
  {
    public:

      EventDispatcherTestsWrapper(EventDispatcher* dispatcher)
        : m_dispatcher(dispatcher)
        {}

      size_t GetObserversVecSize() const
      {
        return m_dispatcher->m_observers.size();
      }

      bool IsListenerAdded(EventListener* _listener) const
      {
        auto observers_vec = m_dispatcher->m_observers;

        return std::find(observers_vec.cbegin(), observers_vec.cend(), _listener) != observers_vec.cend();
      }

    private:

      EventDispatcher* m_dispatcher;
  };
#endif