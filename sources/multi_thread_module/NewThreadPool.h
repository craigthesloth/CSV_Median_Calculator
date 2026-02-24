#pragma once
#include <queue>
#include <vector>
#include <thread>
#include <iostream>
#include <mutex>
#include <future>
#include <functional>
#include <condition_variable>
#include <atomic>
#include <type_traits>

class ThreadPool
{
public:
	explicit ThreadPool(size_t num);
	~ThreadPool();


	ThreadPool(const ThreadPool&) = delete;
	ThreadPool& operator=(const ThreadPool&) = delete;

	template<typename Func, typename...Args>
	auto enqueue(Func&& f, Args&&... args) ->
		std::future<std::invoke_result_t<Func, Args...>>
	{
		using ReturnType = std::invoke_result_t<Func, Args...>;
		auto task = std::make_shared < std::packaged_task<ReturnType()>>(std::bind(std::forward<Func>(f), std::forward<Args>(args)...));
		std::future<ReturnType> result = task->get_future();
		{
			std::lock_guard<std::mutex> lock(m_mutex);
			m_tasks.emplace([task]() {(*task)(); });
		}
		m_condition.notify_one();
		return result;
	}
protected:
	void setJoin();

private:
	std::vector<std::thread> m_threads;
	std::mutex m_mutex;
	std::queue<std::function<void()>> m_tasks;
	std::atomic<bool> m_stop{ false };
	std::condition_variable m_condition;
};