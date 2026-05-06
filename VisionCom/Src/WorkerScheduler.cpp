#include "StdAfx.h"
#include "WorkerScheduler.h"

#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <vector>
#include <deque>

namespace VisionCom
{
    struct WorkerScheduler::Impl
    {
        std::vector<boost::thread*> m_threads;
        std::deque<WorkTask> m_queue;
        boost::mutex m_mutex;
        boost::condition_variable m_cv;
        bool m_running;
        size_t m_threadCount;

        Impl(size_t count) : m_threadCount(count), m_running(false) {}

        ~Impl() {
            Stop();
        }

        void Stop() {
            {
                boost::unique_lock<boost::mutex> lk(m_mutex);
                m_running = false;
                m_cv.notify_all();
            }

            for (size_t i = 0; i < m_threads.size(); ++i) {
                try {
                    if (m_threads[i]->joinable()) m_threads[i]->join();
                } catch(...) {}
            }

            for (size_t i = 0; i < m_threads.size(); ++i) {
                delete m_threads[i];
            }
            m_threads.clear();
        }

        void RunWorker() {
            while (true) {
                WorkTask task;
                {
                    boost::unique_lock<boost::mutex> lk(m_mutex);
                    while (m_running && m_queue.empty()) {
                        m_cv.wait(lk);
                    }
                    if (!m_running && m_queue.empty()) break;
                    task = m_queue.front();
                    m_queue.pop_front();
                }
                try {
                    task();
                } catch(...) {

                }
            }
        }
    };


    WorkerScheduler::WorkerScheduler(size_t threadCount)
        : m_pImpl(new Impl(threadCount))
    {
        
    }


    WorkerScheduler::~WorkerScheduler() {
        if(m_pImpl)
        {
            delete m_pImpl;
            m_pImpl = NULL;
        }
    }


    void WorkerScheduler::Start()
    {
        boost::unique_lock<boost::mutex> lk(m_pImpl->m_mutex);
        if (m_pImpl->m_running) return;
        m_pImpl->m_running = true;

        if (m_pImpl->m_threads.empty())
        {
            for (size_t i = 0; i < m_pImpl->m_threadCount; ++i)
            {
                boost::thread* t = new boost::thread(boost::bind(&WorkerScheduler::Impl::RunWorker, m_pImpl));
                m_pImpl->m_threads.push_back(t);
            }
        }
    }

    void WorkerScheduler::Stop()
    {
        m_pImpl->Stop();
    }

    void WorkerScheduler::Enqueue(WorkTask t)
    {
        boost::unique_lock<boost::mutex> lock(m_pImpl->m_mutex);
        m_pImpl->m_queue.push_back(t);
        m_pImpl->m_cv.notify_one();
    }

    void WorkerScheduler::Schedule(const SchedulerTask& task, int delayMs)
    {
        Enqueue(task);
    }
}
