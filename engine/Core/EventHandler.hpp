#pragma once

#include <functional>

namespace Events
{
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
  class Listener : public Observer
  {

  public:

    Listener() = default;
    ~Listener() = default;

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
  class Dispatcher
  {

  public:

    Dispatcher() = default;
    ~Dispatcher() = default;

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
    friend class DispatcherTestsWrapper;
    #endif
  };

  #if (defined TESTS)
  class DispatcherTestsWrapper
  {
  public:

    DispatcherTestsWrapper(Dispatcher* _dispatcher)
      : m_dispatcher(_dispatcher)
    {}

    size_t GetObserversVecSize() const
    {
      return m_dispatcher->m_observers.size();
    }

    bool IsListenerAdded(Listener* _listener) const
    {
      auto observers_vec = m_dispatcher->m_observers;

      return std::find(observers_vec.cbegin(), observers_vec.cend(), _listener) != observers_vec.cend();
    }

  private:

    Dispatcher* m_dispatcher;
  };
  #endif
};