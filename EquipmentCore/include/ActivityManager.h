#pragma once

#include "EC_API.h"
#include "ActivityBuilderBase.h"
#include "AsyncExecutor.h"
#include "IResultSink.h"

#include <memory>
#include <string>
#include <unordered_map>
#include <functional>
#include <mutex>
#include <atomic>
#include <cstdint>

namespace EC
{
    /// <summary>
    /// Activity 정보 (등록된 각 Activity의 상태 및 제어 인터페이스)
    /// running	의미	설명
    /// true	실행 중	해당 이름의 Activity가 등록되어 있고, AsyncExecutor::Start()가 성공적으로 호출되어 스레드가 동작 중
    /// false	실행 중 아님	다음 중 하나 : ① Activity가 등록되지 않음, ② 등록만 되고 아직 StartActivity()가 호출되지 않음, ③ StopActivity()로 중단됨, ④ Activity 실행이 완료되어 종료됨
    /// </summary>
    struct EC_API ActivityEntry
    {
        std::string            name;          // Activity 이름 (ex: "Load1", "Load2")
        ActivityBuilderPtr     builder;       // Activity Builder
        AsyncExecutorPtr       runner;        // 비동기 실행기
        ContextPtr             ctx;           // 실행 컨텍스트
        std::atomic<bool>      running;       // 실행 중 여부

        ActivityEntry(const std::string& name)
            : name(name)
            , running(false)
        {
        }
    };

    using ActivityEntryPtr = std::shared_ptr<ActivityEntry>;
    using ActivityId = std::uint64_t;

    /// <summary>
    /// Activity 실행 결과를 통지받을 Observer
    /// </summary>
    using ActivityObserver = std::function<void(const std::string& activityName, int requestId, const std::vector<std::string>& results)>;

    /// <summary>
    /// ActivityManager — 다수의 Activity(Load1, Load2, Unload, Sort 등)를 
    /// 동적으로 등록/실행/중단/관리하는 레지스트리입니다.
    /// 
    /// 사용 예:
    ///   auto mgr = std::make_shared<ActivityManager>();
    ///   
    ///   // 1. Builder 등록
    ///   mgr->RegisterActivity<CLoad1ActivityBuilder>("Load1");
    ///   mgr->RegisterActivity<CLoad2ActivityBuilder>("Load2");
    ///   
    ///   // 2. 전체 실행
    ///   mgr->ResumeAll();
    ///
    ///   // 3. 전체 대기
    ///   mgr->PauseAll();
    ///
    ///   // 4. 전체 중단
    ///   mgr->StopAll();
    ///
    ///   // 5. 개별 실행
    ///   mgr->ResumeActivity("Load1");
    ///
    ///   // 6. 개별 대기
    ///   mgr->PauseActivity("Load1");
    /// 
    ///   // 7. 개별 중단
    ///   mgr->StopActivity("Load1");
    ///   
    ///   // 8. 결과 관찰
    ///   mgr->AddObserver([](auto& name, int id, auto& results) {
    ///       std::cout << name << " completed" << std::endl;
    ///   });
    /// </summary>
    class EC_API ActivityManager : public IResultSink
    {
    public:
        ActivityManager();
        ~ActivityManager() override;

        // ============ Activity 등록 ============

        /// <summary>
        /// 템플릿 Builder 타입으로 Activity 등록
        /// </summary>
        template <typename BuilderType>
        bool RegisterActivity(const std::string& name)
        {
            static_assert(std::is_base_of_v<ActivityBuilderBase, BuilderType>,
                "BuilderType must derive from ActivityBuilderBase");

            auto builder = std::make_shared<BuilderType>();
            return RegisterActivityWithBuilder(name, std::static_pointer_cast<ActivityBuilderBase>(builder));
        }

        /// <summary>
        /// 이미 생성된 Builder로 Activity 등록
        /// </summary>
        bool RegisterActivityWithBuilder(const std::string& name, ActivityBuilderPtr builder);

        /// <summary>
        /// 등록된 Activity 제거
        /// </summary>
        bool UnregisterActivity(const std::string& name);

        /// <summary>
        /// 모든 Activity 제거
        /// </summary>
        void Clear();

        // ============ 실행 제어 ============

        /// <summary>
        /// 특정 Activity 실행
        /// </summary>
        bool StartActivity(const std::string& name);

/// <summary>
        /// 모든 등록된 Activity 실행
        /// </summary>
        void StartAll();

/// <summary>
        /// 모든 Activity 실행 재개 (Resume)
        /// 각 Activity의 Context에 Resume 상태를 설정합니다.
        /// </summary>
        void ResumeAll();

        /// <summary>
        /// 특정 Activity 실행 재개 (Resume)
        /// </summary>
        bool ResumeActivity(const std::string& name);

        /// <summary>
        /// 모든 Activity 일시 정지 (Pause)
        /// 각 Activity의 Context에 Pause 상태를 설정합니다.
        /// Task 실행 시 isPause()가 true이면 TR_KEEP 반환하여 대기합니다.
        /// </summary>
        void PauseAll();

        /// <summary>
        /// 특정 Activity 일시 정지 (Pause)
        /// </summary>
        bool PauseActivity(const std::string& name);

        /// <summary>
        /// 특정 Activity 중단
        /// </summary>
        void StopActivity(const std::string& name);

        /// <summary>
        /// 모든 Activity 중단
        /// </summary>
        void StopAll();

        /// <summary>
        /// 특정 Activity 실행 중 여부
        /// </summary>
        bool IsRunning(const std::string& name) const;

        /// <summary>
        /// 등록된 Activity 수 반환
        /// </summary>
        size_t GetActivityCount() const;

        /// <summary>
        /// 등록된 Activity 이름 목록 반환
        /// </summary>
        std::vector<std::string> GetActivityNames() const;

        // ============ Observer ============

        /// <summary>
        /// Activity 실행 결과를 통지받을 Observer 등록
        /// </summary>
        ActivityId AddObserver(ActivityObserver observer);

        /// <summary>
        /// Observer 제거
        /// </summary>
        bool RemoveObserver(ActivityId id);

        /// <summary>
        /// 모든 Observer 제거
        /// </summary>
        void ClearObservers();

        // ============ IResultSink 구현 ============
        void NotifyResult(int requestId, const std::vector<std::string>& results) override;

        /// <summary>
        /// 특정 Activity의 Context 반환 (파라미터 설정/조회용)
        /// </summary>
        ContextPtr GetContext(const std::string& name) const;

    private:
        ActivityEntryPtr FindActivity(const std::string& name) const;

        // IResultSink → ActivityManager로 결과 전달될 때
        // 어떤 Activity의 결과인지 찾기 위한 매핑
        using SinkId = int;
        mutable std::mutex                         m_mutex;
        std::unordered_map<std::string, ActivityEntryPtr> m_activities;

        // Observer
        mutable std::mutex                         m_observerMutex;
        std::unordered_map<ActivityId, ActivityObserver> m_observers;
        std::atomic<ActivityId>                    m_nextObserverId{1};

        // RequestId → ActivityName 매핑 (비동기 결과 수신용)
        std::unordered_map<SinkId, std::string>    m_requestToActivity;
        std::atomic<SinkId>                        m_nextRequestId{1000};
    };

    using ActivityManagerPtr = std::shared_ptr<ActivityManager>;
}
