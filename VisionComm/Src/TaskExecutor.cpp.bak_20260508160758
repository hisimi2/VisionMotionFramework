#include "StdAfx.h"
#include "TaskExecutor.h"

#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <deque>
#include <chrono>

namespace VisionCom
{
    struct TaskExecutor::Impl
    {
        std::vector<std::thread> m_threads;
        std::deque<WorkTask> m_queue;
        std::mutex m_mutex;
        std::condition_variable m_cv;
        bool m_running;
        size_t m_threadCount;

        explicit Impl(size_t count)
            : m_running(false)
            , m_threadCount(count)
        {
        }

        ~Impl()
        {
            Stop();
        }

        void Stop()
        {
            {
                std::unique_lock<std::mutex> lk(m_mutex);
                m_running = false;
                m_cv.notify_all();
            }

            for (auto& t : m_threads)
            {
                try
                {
                    if (t.joinable())
                    {
                        t.join();
                    }
                }
                catch (...)
                {
                }
            }

            m_threads.clear();
        }

        void RunWorker()
        {
            while (true)
            {
                WorkTask task;
                {
                    std::unique_lock<std::mutex> lk(m_mutex);
                    m_cv.wait(lk, [this]() { return !m_running || !m_queue.empty(); });

                    if (!m_running && m_queue.empty())
                    {
                        break;
                    }

                    task = std::move(m_queue.front());
                    m_queue.pop_front();
                }

                try
                {
                    if (task)
                    {
                        task();
                    }
                }
                catch (...)
                {
                }
            }
        }
    };

    TaskExecutor::TaskExecutor(size_t threadCount)
        : m_pImpl(std::make_unique<Impl>(threadCount))
    {
    }

    TaskExecutor::~TaskExecutor()
    {
    }

    void TaskExecutor::Start()
    {
        std::unique_lock<std::mutex> lk(m_pImpl->m_mutex);
        if (m_pImpl->m_running)
        {
            return;
        }

        m_pImpl->m_running = true;

        if (m_pImpl->m_threads.empty())
        {
            for (size_t i = 0; i < m_pImpl->m_threadCount; ++i)
            {
                m_pImpl->m_threads.emplace_back(&TaskExecutor::Impl::RunWorker, m_pImpl.get());
            }
        }
    }

    void TaskExecutor::Stop()
    {
        m_pImpl->Stop();
    }

    void TaskExecutor::Enqueue(WorkTask t)
    {
        std::unique_lock<std::mutex> lock(m_pImpl->m_mutex);
        m_pImpl->m_queue.push_back(std::move(t));
        m_pImpl->m_cv.notify_one();
    }

    void TaskExecutor::Schedule(const SchedulerTask& task, int delayMs)
    {
        if (!task)
        {
            return;
        }

        if (delayMs <= 0)
        {
            Enqueue(task);
            return;
        }

        Enqueue([task, delayMs]() {
            std::this_thread::sleep_for(std::chrono::milliseconds(delayMs));
            task();
        });
    }
}
