#pragma once

#include <chrono>
#include <string>
#include <memory>
#include "EquipmentCore.h"
#include "ThreadBase.h"

class Load2Parts;

namespace OperationThread
{
    /// <summary>
    /// Load2 Pick & Place 시퀀스 구현
    /// EquipmentCore의 ISequenceExecutable 인터페이스를 구현
    /// </summary>
    class ThreadLoad2 : public ThreadBase
    {
    public:
        enum class PickPlaceStep 
        {
            RailOpen = 0,
            MovePickPositionXZ,
            PreciserDown,
            VacuumOn,
            MoveSafeZAfterPick,
            MovePlacePositionXZ,
            PreciserUp,
            VacuumOff,
            MoveSafeZAfterPlace,
            CheckRepeat,
            Complete
        };

        ThreadLoad2(LPVOID parts, int repeatCount = 0);
        ~ThreadLoad2() override;

        // ISequenceExecutable 구현
        void OnInitialize() override;
        bool OnPoll() override;
        void OnCleanup() override;
        void OnError(const std::string& errorMsg) override;

        // 파라미터 설정 메서드
        void SetPickPosition(double x, double z);
        void SetPlacePosition(double x, double z);
        void SetSafeZ(double z);
        void SetMoveTimeout(long timeoutMs);
        void SetVacuumIndex(int index);

        // 상태 조회 메서드
        PickPlaceStep GetCurrentStep() const;
        bool HandleStep(int step) override;
        int GetStepCount() const override { return static_cast<int>(PickPlaceStep::Complete) + 1; }

        // 에러 메시지 조회
        std::string GetLastError() const { return m_lastError; }

    private:
    // 단계별 처리 함수
        bool HandleRailOpen();
        bool HandleMovePickPositionXZ();
        bool HandlePreciserDown();
        bool HandleVacuumOn();
        bool HandleMoveSafeZAfterPick();
        bool HandleMovePlacePositionXZ();
        bool HandlePreciserUp();
        bool HandleVacuumOff();
        bool HandleMoveSafeZAfterPlace();
        bool HandleCheckRepeat();
        bool HandleComplete();

        // 유틸리티 메서드
        void MoveToNextStep();
        bool IsStepTimeout() const;
        void LogStep(const std::string& message);

        // 멤버 변수
        Load2Parts* m_parts;
        int m_repeatCount;
        int m_currentIteration;
        int m_successCount;
        PickPlaceStep m_currentStep;
        long m_moveTimeoutMs;

        // 위치 파라미터
        double m_pickX, m_pickZ;
        double m_placeX, m_placeZ;
        double m_safeZ;

        // 액추에이터 인덱스
        int m_vacuumIndex;

        // 상태
        bool m_initialized;
        std::string m_lastError;
        std::chrono::steady_clock::time_point m_stepStartTime;
    };

    using ThreadLoad2Ptr = std::shared_ptr<ThreadLoad2>;

} // namespace OperationThread
