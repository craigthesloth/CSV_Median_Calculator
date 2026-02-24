#include "NewThreadPool.h"


ThreadPool::ThreadPool(size_t num) 
{
	for (size_t i = 0; i < num; ++i)
	{
		m_threads.emplace_back([this] {
			while (true)
			{
				std::function<void()> newTask;
				{
					std::unique_lock<std::mutex> lock(m_mutex);

					m_condition.wait(lock, [this] {return m_stop || !m_tasks.empty(); });
					if (m_stop && m_tasks.empty()) { return; }
					newTask = std::move(m_tasks.front());
					m_tasks.pop();
				}
				newTask();
			}
			});
	}
}

ThreadPool::~ThreadPool() 
{
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		m_stop = true;
	}
	m_condition.notify_all();
	setJoin();
}

void ThreadPool::setJoin()
{
	if(!m_threads.empty())
	{
		for (auto& t : m_threads)
		{
		if(t.joinable())
		{
			t.join();
		}
		}
	}
}