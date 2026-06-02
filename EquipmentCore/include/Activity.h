#pragma once
#include "EC_API.h"
#include "IActivity.h" 
#include "ITask.h"

#include <vector>
#include <string>
#include <mutex>
#include <thread>

namespace EC
{
    class EC_API Activity : public IActivity
    {
    public:
        explicit Activity(const std::string& name = "");
        ~Activity() override;

        bool Execute(Context& context) override;
        void Abort() override;
        std::string GetActivityName() const override;

        void AddTask(TaskPtr step) override;
        void SetPollIntervalMs(int ms) { m_pollIntervalMs = ms; }

        Activity(const Activity&) = delete;
        Activity& operator=(const Activity&) = delete;

    private:
        std::vector<TaskPtr> m_tasks;
        mutable std::mutex   m_taskMutex;

        int         m_pollIntervalMs;
        std::string m_ActivityName;
    };
}
