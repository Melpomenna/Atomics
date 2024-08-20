#include "pch.h"

#include <Atomics.hpp>
#include <thread>

TEST(AtomicIntTest,Test_1)
{
	AtomicInt value{ 5 };

	ASSERT_EQ(value.load(), 5);
}

TEST(AtomicIntTest,Test_2)
{
	AtomicInt value;

	value.store(20);

	ASSERT_EQ(value.load(), 20);
}

TEST(AtomicIntTest,Test_3)
{
	AtomicInt value;

	value.store(20);
	value.add(50);

	ASSERT_EQ(value.load(), 70);
}

TEST(AtomicIntTest,Test_4)
{
	AtomicInt value;

	value.store(20);
	value.sub(50);

	ASSERT_EQ(value.load(), -30);
}

TEST(AtomicIntTest,Test_5)
{
	const int base = 50;
	AtomicInt value{ base };
	std::atomic_int res{ base };
	constexpr auto source = 100;

	const auto size = std::thread::hardware_concurrency();
	std::vector<std::thread> workers{};
	for (auto i = 0U; i < size; ++i)
	{
		workers.emplace_back([](AtomicInt& value, std::atomic_int& res, int source)
			{
				value.add(source);
				res.fetch_add(source);
			}, std::ref(value), std::ref(res), source);
	}

	for (auto&& worker : workers)
	{
		if (worker.joinable())
			worker.join();
	}

	ASSERT_EQ(value.load(), static_cast<int>(base + source * size));
	ASSERT_EQ(value.load(), res.load());
}

TEST(AtomicIntTest,Test_6)
{
	const int base = 50;
	AtomicInt value{ base };
	std::atomic_int res{ base };
	constexpr int source = 100;

	const auto size = std::thread::hardware_concurrency();
	std::vector<std::thread> workers{};
	for (auto i = 0U; i < size; ++i)
	{
		workers.emplace_back([](AtomicInt& value, std::atomic_int& res, int source)
			{
				value.sub(source);
				res.fetch_sub(source);
			}, std::ref(value), std::ref(res), source);
	}

	for (auto&& worker : workers)
	{
		if (worker.joinable())
			worker.join();
	}

	ASSERT_EQ(value.load(), base - source * static_cast<int>(size));
	ASSERT_EQ(value.load(), res.load());
}

TEST(AtomicIntTest,Test_7)
{
	const int base = 50;
	AtomicInt value{ base };
	std::atomic_int res{ base };
	constexpr int source = 100;

	const auto size = std::thread::hardware_concurrency();
	std::vector<std::thread> workers{};
	for (auto i = 0U; i < size; ++i)
	{
		workers.emplace_back([](AtomicInt& value, std::atomic_int& res, int source)
			{
				value.sub(source);
				res.fetch_sub(source);
			}, std::ref(value), std::ref(res), source);
	}

	for (auto&& worker : workers)
	{
		if (worker.joinable())
			worker.join();
	}

	ASSERT_EQ(value.load(), base - source * static_cast<int>(size));
	ASSERT_EQ(value.load(), res.load());
}

TEST(AtomicIntTest,Test_8)
{
	const int base = 50;
	AtomicInt value{ base };
	std::atomic_int res{ base };
	constexpr int source = 100;

	const auto size = std::thread::hardware_concurrency();
	std::vector<std::thread> workers{};
	for (auto i = 0U; i < size; ++i)
	{
		workers.emplace_back([](AtomicInt& value, std::atomic_int& res, int source)
			{
				std::this_thread::sleep_for(std::chrono::seconds(1));
				value.sub(source);
				res.fetch_sub(source);
			}, std::ref(value), std::ref(res), source);
	}

	std::this_thread::sleep_for(std::chrono::seconds(5));
	for (auto&& worker : workers)
	{
		if (worker.joinable())
			worker.join();
	}

	ASSERT_EQ(value.load(), base - source * static_cast<int>(size));
	ASSERT_EQ(value.load(), res.load());
}

TEST(AtomicBoolTest,Test_1)
{
	AtomicBool flag{false};

	ASSERT_EQ(*flag, bool{});
}

TEST(AtomicBoolTest,Test_2)
{
	AtomicBool flag{ true };

	ASSERT_EQ(flag.load(), true);
}

TEST(AtomicBoolTest,Test_3)
{
	AtomicBool flag{ true };

	ASSERT_EQ(flag.load(), true);

	flag.store(false);

	ASSERT_EQ(flag.load(), false);
}

TEST(AtomicBoolTest,Test_4)
{
	AtomicBool flag{ false };
	AtomicBool flag2{ true };

	ASSERT_EQ(flag.load(), false);

	flag = std::move(flag2);

	ASSERT_EQ(flag.load(), true);
#pragma warning(disable:26800)
	ASSERT_EQ(flag2.load(), false);
#pragma warning(default:26800)
}

TEST(AtomicBoolTest,Test_5)
{
	AtomicBool flag{ false };
	AtomicBool flag2{ true };

	ASSERT_EQ(flag.load(), false);

	flag = flag2;

	ASSERT_EQ(flag.load(), true);
	ASSERT_EQ(flag2.load(), true);
}
