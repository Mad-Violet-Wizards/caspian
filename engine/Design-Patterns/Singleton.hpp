#pragma once

template<class C>
class Singleton
{
  public:

    using class_type = C;

    template<typename... Args>
    static void Create(Args&&... args)
    {
        SingletonData& data = GetData();

        if (!data.m_valid)
        {
          data.m_instance = std::make_unique<class_type>(std::forward<Args>(args)...);
          data.m_valid = true;
        }
    }

    static void Destroy()
    {
      SingletonData& data = GetData();

      if (data.m_valid)
      {
        data.m_instance->~class_type();
        data.m_instance = nullptr;
        data.m_valid = false;
      }
    }

    static class_type& Instance()
    {
      SingletonData& data = GetData();

      // Tricky, as we don't know if the user provided an default ctor.
      if (!data.m_valid)
        Create();

      return *data.m_instance;
    }

  public:

    ~Singleton() = default;

    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;

    Singleton(Singleton&&) = delete;
    Singleton& operator=(Singleton&&) = delete;

  private:

    Singleton() = default;

  private:

    struct SingletonData
    {
      SingletonData()
        : m_valid(false)
      {

      }

      std::unique_ptr<class_type> m_instance;
      bool m_valid : 1;
    };

    static SingletonData& GetData()
    {
      static SingletonData s_data;

      return s_data;
    }
}