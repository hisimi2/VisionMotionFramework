#pragma once

#include "ActivityManager.h"

#include <string>
#include <vector>

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
    ///   mgr.RunAll();           // 모두 실행
    ///   mgr.PauseAll();         // 모두 일시정지
    ///   mgr.StopAll();          // 모든 완전종료
    ///   mgr.RunActivity("Load1");   // Load1 실행
    ///   mgr.PauseActivity("Load1"); // Load1 일시정지
    ///   mgr.StopActivity("Load1");  // Load1 완전종료
    
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
        /// 모든 등록된 Activity 실행 (처음 실행 또는 재개)
        /// </summary>
        void RunAll();

        /// <summary>
        /// 모든 중단
        /// </summary>
        void PauseAll();

        /// <summary>
        /// 모든 중단
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
