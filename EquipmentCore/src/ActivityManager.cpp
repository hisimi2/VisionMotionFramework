#include "stdafx.h"
#include "ActivityManager.h"
#include "Context.h"
#include "IActivity.h"

#include <memory>
#include <string>
#include <algorithm>
#include <exception>

namespace EC
{
    ActivityManager::ActivityManager()
    {
    }

    ActivityManager::~ActivityManager()
    {
        StopAll();
        Clear();
        ClearObservers();
    }

    // ============ Activity 등록 ============

    bool ActivityManager::RegisterActivityWithBuilder(const std::string& name, ActivityBuilderPtr builder)
    {
        if (!builder)
            return false;

        if (name.empty())
            return false;

        std::lock_guard<std::mutex> lock(m_mutex);

        // 중복 등록 방지
        if (m_activities.find(name) != m_activities.end())
        {
            // 이미 등록된 경우 교체 (기존 중단 후)
            auto& entry = m_activities[name];
            if (entry->running.load())
            {
                if (entry->runner)
                {
                    entry->runner->Stop();
                    entry->runner->WaitForCompletion(3000);
                }
                entry->running.store(false);
            }
        }

        auto entry = std::make_shared<ActivityEntry>(name);
        entry->builder = builder;
        m_activities[name] = entry;

        return true;
    }

    bool ActivityManager::UnregisterActivity(const std::string& name)
    {
        std::lock_guard<std::mutex> lock(m_mutex);

        auto it = m_activities.find(name);
        if (it == m_activities.end())
            return false;

        auto& entry = it->second;

        // 실행 중이면 중단
        if (entry->running.load())
        {
            if (entry->runner)
            {
                entry->runner->Stop();
                entry->runner->WaitForCompletion(3000);
            }
            entry->running.store(false);
        }

        m_activities.erase(it);
        return true;
    }

    void ActivityManager::Clear()
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_activities.clear();
    }

    // ============ 실행 제어 ============

    bool ActivityManager::StartActivity(const std::string& name)
    {
        auto entry = FindActivity(name);
        if (!entry)
            return false;

        if (entry->running.load())
        {
            // 이미 실행 중이면 재시작을 위해 중단
            if (entry->runner)
            {
                entry->runner->Stop();
                entry->runner->WaitForCompletion(3000);
            }
            entry->running.store(false);
        }

        // Context 생성
        auto ctx = std::make_shared<Context>();

        // Builder로 파라미터 설정
        try
        {
            entry->builder->ConfigureParams(ctx);
        }
        catch (const std::exception& ex)
        {
            ctx->SetLastError(std::string("ConfigureParams failed: ") + ex.what());
            return false;
        }
        catch (...)
        {
            ctx->SetLastError("Unknown exception in ConfigureParams");
            return false;
        }

        // AsyncExecutor 생성
        auto runner = std::make_shared<AsyncExecutor>();
        runner->SetResultSink(this);

        // Activity 생성
        std::unique_ptr<IActivity> activity;
        try
        {
            activity = entry->builder->Create();
            if (!activity)
            {
                ctx->SetLastError("Builder::Create() returned null");
                return false;
            }
        }
        catch (const std::exception& ex)
        {
            ctx->SetLastError(std::string("Activity creation failed: ") + ex.what());
            return false;
        }
        catch (...)
        {
            ctx->SetLastError("Unknown exception creating activity");
            return false;
        }

        // 실행
        if (!runner->Start(std::move(activity), ctx))
        {
            ctx->SetLastError("AsyncExecutor::Start() failed");
            return false;
        }

        // 상태 업데이트
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            entry->runner = runner;
            entry->ctx = ctx;
            entry->running.store(true);
        }

        return true;
    }

    void ActivityManager::StartAll()
    {
        std::vector<std::string> names;
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            for (const auto& kv : m_activities)
            {
                names.push_back(kv.first);
            }
        }

        for (const auto& name : names)
        {
            StartActivity(name);
        }
    }

    void ActivityManager::StopActivity(const std::string& name)
    {
        auto entry = FindActivity(name);
        if (!entry)
            return;

        if (entry->runner)
        {
            entry->runner->Stop();
            entry->runner->WaitForCompletion(3000);
        }
        entry->running.store(false);
    }

    void ActivityManager::StopAll()
    {
        std::vector<AsyncExecutorPtr> runners;

        {
            std::lock_guard<std::mutex> lock(m_mutex);
            for (auto& kv : m_activities)
            {
                auto& entry = kv.second;
                if (entry->runner)
                {
                    entry->runner->Stop();
                    runners.push_back(entry->runner);
                }
                entry->running.store(false);
            }
        }

        // 모든 Runner 종료 대기
        for (auto& runner : runners)
        {
            if (runner)
            {
                runner->WaitForCompletion(3000);
            }
        }
    }

    bool ActivityManager::IsRunning(const std::string& name) const
    {
        auto entry = FindActivity(name);
        return entry && entry->running.load();
    }

    size_t ActivityManager::GetActivityCount() const
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_activities.size();
    }

    std::vector<std::string> ActivityManager::GetActivityNames() const
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        std::vector<std::string> names;
        names.reserve(m_activities.size());
        for (const auto& kv : m_activities)
        {
            names.push_back(kv.first);
        }
        return names;
    }

    // ============ Observer ============

    ActivityId ActivityManager::AddObserver(ActivityObserver observer)
    {
        if (!observer)
            return 0;

        const ActivityId id = m_nextObserverId.fetch_add(1);
        {
            std::lock_guard<std::mutex> lk(m_observerMutex);
            m_observers[id] = std::move(observer);
        }
        return id;
    }

    bool ActivityManager::RemoveObserver(ActivityId id)
    {
        std::lock_guard<std::mutex> lk(m_observerMutex);
        return m_observers.erase(id) > 0;
    }

    void ActivityManager::ClearObservers()
    {
        std::lock_guard<std::mutex> lk(m_observerMutex);
        m_observers.clear();
    }

    // ============ IResultSink 구현 ============

    void ActivityManager::NotifyResult(int requestId, const std::vector<std::string>& results)
    {
        // RequestId → ActivityName 매핑 조회
        std::string activityName;
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            auto it = m_requestToActivity.find(requestId);
            if (it != m_requestToActivity.end())
            {
                activityName = it->second;
            }
        }

        // Observer들에게 통지
        std::vector<ActivityObserver> snapshot;
        {
            std::lock_guard<std::mutex> lk(m_observerMutex);
            snapshot.reserve(m_observers.size());
            for (auto& kv : m_observers)
            {
                if (kv.second)
                    snapshot.push_back(kv.second);
            }
        }

        for (auto& cb : snapshot)
        {
            try
            {
                cb(activityName, requestId, results);
            }
            catch (...)
            {
                // ignore observer exceptions
            }
        }
    }

    // ============ Context ============

    ContextPtr ActivityManager::GetContext(const std::string& name) const
    {
        auto entry = FindActivity(name);
        if (entry)
            return entry->ctx;
        return nullptr;
    }

    // ============ 내부 헬퍼 ============

    ActivityEntryPtr ActivityManager::FindActivity(const std::string& name) const
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        auto it = m_activities.find(name);
        if (it != m_activities.end())
            return it->second;
        return nullptr;
    }
}