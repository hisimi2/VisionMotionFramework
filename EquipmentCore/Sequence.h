#pragma once
#include "EC_API.h"
#include "ISequence.h" 
#include "ITask.h"

#include <vector>
#include <string>
#include <mutex>
#include <condition_variable>
#include <atomic>

namespace EC
{
    class EC_API Sequence : public ISequence
    {
    public:
        explicit Sequence(const std::string& name = "");
        ~Sequence() override;

        bool Execute(Context& context) override;
        void Abort() override;
        std::string GetSequenceName() const override;
        std::string GetTaskName() const override;

        void AddTask(TaskPtr step) override;

        // 폴링 간격(밀리초). 필요하면 런타임에 조정 가능.
        void SetPollIntervalMs(int ms) { m_pollIntervalMs = ms; }

        Sequence(const Sequence&) = delete;
        Sequence& operator=(const Sequence&) = delete;

    private:
        std::vector<TaskPtr> m_tasks;
        std::mutex m_mutex;
        std::condition_variable m_cv;
        std::atomic<bool> m_abortRequested; 
        
        int m_pollIntervalMs;
        std::string m_SequenceName;

        TaskPtr m_curTask;
    };
} 
