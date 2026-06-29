#pragma once
#include "VMF_API.h"
#include "ISequence.h" 
#include "ITask.h"
#include "CompatUtils.h" 

#include <vector>
#include <string>
#include <mutex>
#include <condition_variable>
#include <atomic>

namespace VMF 
{
    class IActuator;

    // C++14 호환 모던 C++ 클래스
    class VMF_API Sequence : public ISequence
    {
    public:
        explicit Sequence(const std::string& name = "");
        
        ~Sequence() override;

        bool Execute(Context& context, IActuator* actuator) override;
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
        std::string m_sequenceName;
        std::string m_taskName;
    };

} // namespace VMF

