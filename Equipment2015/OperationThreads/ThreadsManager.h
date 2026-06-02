#pragma once

#include "ActivityManager.h"
#include "Load1ActivityBuilder.h"
#include "Load2ActivityBuilder.h"

#include <memory>
#include <string>
#include <vector>
#include <functional>

namespace OperationThread
{
    using namespace EC;

    /// <summary>
    /// ThreadsManager — ActivityManager를 통해 다수의 Activity(Load1, Load2, ...)를
    /// 동적으로 등록/실행/중단/관리합니다.
    /// 
    /// 사용 예:
    ///   ThreadsManager mgr;
    ///   mgr.Initialize();       // Load1, Load2 등록
    ///   mgr.StartAll();         // 모두 실행
    ///   mgr.StopActivity("Load1");  // Load1만 중단
    ///   mgr.StopAll();          // 모두 중단
    /// </summary>
    class ThreadsManager
    {
    public:
        ThreadsManager();
        ~ThreadsManager();

        /// <summary>
        /// Activity 초기화 — 모든 Builder를 ActivityManager에 등록
        /// </summary>
        void Initialize();

        /// <summary>
        /// 특정 Activity 실행
        /// </summary>
        bool StartActivity(const std::string& name);

        /// <summary>
        /// 모든 등록된 Activity 실행
        /// </summary>
        void StartAll();

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
        /// 등록된 Activity 이름 목록 반환
        /// </summary>
        std::vector<std::string> GetActivityNames() const;

        /// <summary>
        /// Activity 실행 결과 Observer 등록
        /// </summary>
        ActivityId AddObserver(ActivityObserver observer);

        /// <summary>
        /// Observer 제거
        /// </summary>
        bool RemoveObserver(ActivityId id);

        /// <summary>
        /// ActivityManager 직접 접근 (고급 제어용)
        /// </summary>
        ActivityManager& GetManager();

    private:
        ActivityManager m_mgr;
        bool m_initialized;
    };
}