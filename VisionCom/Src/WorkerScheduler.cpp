#include "StdAfx.h"
#include "WorkerScheduler.h"

// Boost 대신 C++ 표준 라이브러리 사용
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <vector>
#include <deque>

namespace VisionCom
{
    struct WorkerScheduler::Impl
    {
        // C++11: 동적 할당(포인터) 대신 std::thread 객체 보관
        std::vector<std::thread> m_threads;
        std::deque<WorkTask> m_queue;
        std::mutex m_mutex;
        std::condition_variable m_cv;
        bool m_running;
        size_t m_threadCount;

        Impl(size_t count) : m_threadCount(count), m_running(false) {}

        ~Impl() {
            Stop();
        }

        void Stop() {
            {
                std::unique_lock<std::mutex> lk(m_mutex);
                m_running = false;
                m_cv.notify_all();
            }

            for (auto& t : m_threads) {
                try {
                    if (t.joinable()) t.join();
                } catch(...) {}
            }
            m_threads.clear();
        }

        void RunWorker() {
            while (true) {
                WorkTask task;
                {
                    std::unique_lock<std::mutex> lk(m_mutex);
                    m_cv.wait(lk, [this]() { return !m_running || !m_queue.empty(); });
                    
                    if (!m_running && m_queue.empty()) break;
                    
                    task = std::move(m_queue.front());
                    m_queue.pop_front();
                }
                try {
                    if (task) task();
                } catch(...) {

                }
            }
        }
    };


    WorkerScheduler::WorkerScheduler(size_t threadCount)
        : m_pImpl(std::make_unique<Impl>(threadCount)) // [수정] std::make_unique 사용
    {
        
    }

    WorkerScheduler::~WorkerScheduler() {
        // [수정] delete 구문 삭제 -> std::unique_ptr이라 자동 소멸됨
    }


    void WorkerScheduler::Start()
    {
        std::unique_lock<std::mutex> lk(m_pImpl->m_mutex);
        if (m_pImpl->m_running) return;
        m_pImpl->m_running = true;

        if (m_pImpl->m_threads.empty())
        {
            for (size_t i = 0; i < m_pImpl->m_threadCount; ++i)
            {
                m_pImpl->m_threads.emplace_back(&WorkerScheduler::Impl::RunWorker, m_pImpl.get()); // get() 필요
            }
        }
    }

    void WorkerScheduler::Stop()
    {
        m_pImpl->Stop();
    }

    void WorkerScheduler::Enqueue(WorkTask t)
    {
        std::unique_lock<std::mutex> lock(m_pImpl->m_mutex);
        m_pImpl->m_queue.push_back(std::move(t));
        m_pImpl->m_cv.notify_one();
    }

    void WorkerScheduler::Schedule(const SchedulerTask& task, int delayMs)
    {
        Enqueue(task);
    }
}
