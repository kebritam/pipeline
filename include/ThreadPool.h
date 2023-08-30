#pragma once

#include <condition_variable>
#include <thread>
#include <queue>
#include <functional>
#include <future>

namespace pip
{
	class ThreadPool
	{
		std::thread* m_threads;
		std::condition_variable m_cv;
		mutable std::mutex m_mutex;
		std::queue<std::function<void()>> m_tasks;
		bool m_isStopped;
		size_t m_threadCount;

		void task()
		{
			while (!m_isStopped)
			{
				std::function<void()> theTask;
				{
					std::unique_lock lock(m_mutex);
					m_cv.wait(lock, [&]() { return m_isStopped || !m_tasks.empty(); });

					if (m_isStopped && m_tasks.empty())
						return;

					theTask = std::move(m_tasks.front());
					m_tasks.pop();
				}
				theTask();
			}
		}

	public:
		ThreadPool(const ThreadPool& _other) = delete;
		ThreadPool(ThreadPool&& _other) noexcept;
		ThreadPool& operator=(const ThreadPool& _other) = delete;
		ThreadPool& operator=(ThreadPool&& _other) noexcept = delete;

		explicit ThreadPool(const unsigned int _threadCount)
			: m_threads(new std::thread[_threadCount])
			, m_isStopped(false)
			, m_threadCount(_threadCount)
		{
			for (size_t i = 0; i < m_threadCount; ++i)
				m_threads[i] = std::thread([this]() { task(); });
		}

		~ThreadPool()
		{
			{
				std::unique_lock lock(m_mutex);
				m_isStopped = true;
			}
			m_cv.notify_all();
			for (size_t i = 0; i < m_threadCount; ++i)
				if (m_threads[i].joinable())
					m_threads[i].join();
			delete[] m_threads;
		}

		template<typename F, typename... Args>
		auto Enqueue(F&& _func, Args... _args) -> std::future<decltype(_func(_args...))>
		{
			std::function<decltype(_func(_args...))()> task(std::bind(std::forward<F>(_func), std::forward<Args>(_args)...));
			std::shared_ptr<std::promise<decltype(task())>> promise = 
				std::make_shared<std::promise<decltype(task())>>();
			std::future<decltype(task())> res = promise->get_future();
			{
				std::unique_lock lock(m_mutex);
				m_tasks.emplace([promise, task]() mutable
				{
					if constexpr (std::is_void<decltype(task())>::value)
					{
						task();
						promise->set_value();
					}
					else
					{
						promise->set_value(task());
					}
					
				});
			}
			m_cv.notify_one();
			return res;
		}
	};
}
