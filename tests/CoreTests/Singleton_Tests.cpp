#include "../catch.hpp"

#include "../../engine/Design-Patterns/Singleton.hpp"

struct ExampleData
{
	ExampleData() = default;

	ExampleData(int value)
	{
		m_data = new int[10];

		for (int i = 0; i < 10; i++)
			m_data[i] = value;
	}

		~ExampleData()
		{
			delete[] m_data;
			m_data = nullptr;
		}

		int* m_data;
};

using ExampleSingleton = Singleton<ExampleData>;

TEST_CASE("SingletonTests", "[SingletonCreation]")
{
	constexpr int values_set_in_array = 10;
	ExampleSingleton::Create(values_set_in_array);

	auto& data = ExampleSingleton::Instance();

	REQUIRE(data.m_data[0] == values_set_in_array);
	REQUIRE(data.m_data[5] == values_set_in_array);
}

TEST_CASE("SingletonTests", "[SingletonValidCreationFromInstanceMethod]")
{
	REQUIRE_NOTHROW(ExampleSingleton::Instance());
}

TEST_CASE("SingletonTests", "[SingletonDestroy]")
{
	auto& data = ExampleSingleton::Instance();
	ExampleSingleton::Destroy();

	REQUIRE(ExampleSingleton::IsValid() == false);
}

TEST_CASE("SingletonTests", "[SingletonIsValid]")
{
	ExampleSingleton::Create(4);

	REQUIRE(ExampleSingleton::IsValid() == true);

	ExampleSingleton::Destroy();

	REQUIRE(ExampleSingleton::IsValid() == false);

	auto& data = ExampleSingleton::Instance();

	REQUIRE(ExampleSingleton::IsValid() == true);
}


