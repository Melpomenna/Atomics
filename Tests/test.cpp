#include "pch.h"

#include <Atomics.hpp>
#include <thread>
#include <atomic>

TEST(AtomicIntTest, Test_1) {
	AtomicInt value{ 5 };

	ASSERT_EQ(value.load(), 5);
	ASSERT_EQ(value.load(), 5);
}

TEST(AtomicIntTest, Test_2) {
	AtomicInt value;

	value.store(20);

	ASSERT_EQ(value.load(), 20);
}

TEST(AtomicIntTest, Test_3) {
	AtomicInt value;

	value.store(20);
	value.add(50);

	ASSERT_EQ(value.load(), 70);
}

TEST(AtomicIntTest, Test_4) {
	AtomicInt value;

	value.store(20);
	value.sub(50);

	ASSERT_EQ(value.load(), -30);
}

TEST(AtomicIntTest, Test_5) {
	const int base = 50;
	AtomicInt value{base};
	std::atomic_int res{base};
	constexpr auto source = 100;

	const auto size = std::thread::hardware_concurrency();
	std::vector<std::thread> workers{};
	for(auto i = 0U; i < size; ++i)
	{
		workers.emplace_back([](AtomicInt& value, std::atomic_int& res, int source)
		{
			value.add(source);
			res.fetch_add(source);
		}, std::ref(value), std::ref(res), source);
	}

	for(auto&& worker : workers)
	{
		if (worker.joinable())
			worker.join();
	}

	ASSERT_EQ(value.load(), base+source*size);
	ASSERT_EQ(value.load(), res.load());
}

TEST(AtomicIntTest, Test_6) {
	const int base = 50;
	AtomicInt value{base};
	std::atomic_int res{base};
	constexpr int source = 100;

	const auto size = std::thread::hardware_concurrency();
	std::vector<std::thread> workers{};
	for(auto i = 0U; i < size; ++i)
	{
		workers.emplace_back([](AtomicInt& value, std::atomic_int& res, int source)
		{
			value.sub(source);
			res.fetch_sub(source);
		}, std::ref(value), std::ref(res), source);
	}

	for(auto&& worker : workers)
	{
		if (worker.joinable())
			worker.join();
	}

	ASSERT_EQ(value.load(), base-source*static_cast<int>(size));
	ASSERT_EQ(value.load(), res.load());
}

TEST(AtomicIntTest, Test_7) {
	const int base = 50;
	AtomicInt value{base};
	std::atomic_int res{base};
	constexpr auto source = 100;

	const auto size = std::thread::hardware_concurrency();
	std::vector<std::thread> workers{};
	for(auto i = 0U; i < size; ++i)
	{
		workers.emplace_back([](AtomicInt& value, std::atomic_int& res, int source)
		{
			std::this_thread::sleep_for(std::chrono::seconds(1));
			value.add(source);
			res.fetch_add(source);
		}, std::ref(value), std::ref(res), source);
	}

	std::this_thread::sleep_for(std::chrono::seconds(5));
	for(auto&& worker : workers)
	{
		if (worker.joinable())
			worker.join();
	}

	ASSERT_EQ(value.load(), base+source*size);
	ASSERT_EQ(value.load(), res.load());
}

TEST(AtomicIntTest, Test_8) {
	const int base = 50;
	AtomicInt value{base};
	std::atomic_int res{base};
	constexpr int source = 100;

	const auto size = std::thread::hardware_concurrency();
	std::vector<std::thread> workers{};
	for(auto i = 0U; i < size; ++i)
	{
		workers.emplace_back([](AtomicInt& value, std::atomic_int& res, int source)
		{
			std::this_thread::sleep_for(std::chrono::seconds(1));
			value.sub(source);
			res.fetch_sub(source);
		}, std::ref(value), std::ref(res), source);
	}

	std::this_thread::sleep_for(std::chrono::seconds(5));
	for(auto&& worker : workers)
	{
		if (worker.joinable())
			worker.join();
	}

	ASSERT_EQ(value.load(), base-source*static_cast<int>(size));
	ASSERT_EQ(value.load(), res.load());
}