#pragma once

#include <string>
#include <memory>
#include "TaskBase.h"

class Load2Parts;

namespace OperationThread
{
    /// <summary>
    /// Load2 Pick & Place 시퀀스 구현
    /// EC::TaskBase 기반 상태 기계 구현
    /// </summary>
    class ThreadLoad2 : public EC::TaskBase
    {
    public:
        enum Substep
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

        std::string GetName() const override { return "ThreadLoad2"; }

        // 파라미터 설정 메서드
        void SetPickPosition(double x, double z);
        void SetPlacePosition(double x, double z);
        void SetSafeZ(double z);
        void SetMoveTimeout(long timeoutMs);
        void SetVacuumIndex(int index);

        // 상태 조회 메서드
        Substep GetCurrentStep() const { return static_cast<Substep>(GetState()); }

        // 에러 메시지 조회
        std::string GetLastError() const { return m_lastError; }

    protected:
        void OnInitialize(EC::Context& ctx) override;
        EC::TaskResult OnPoll(EC::Context& ctx) override;

    private:
        // 단계별 처리 함수 (TaskResult 반환)
        EC::TaskResult HandleRailOpen(EC::Context& ctx);
        EC::TaskResult HandleMovePickPositionXZ(EC::Context& ctx);
        EC::TaskResult HandlePreciserDown(EC::Context& ctx);
        EC::TaskResult HandleVacuumOn(EC::Context& ctx);
        EC::TaskResult HandleMoveSafeZAfterPick(EC::Context& ctx);
        EC::TaskResult HandleMovePlacePositionXZ(EC::Context& ctx);
        EC::TaskResult HandlePreciserUp(EC::Context& ctx);
        EC::TaskResult HandleVacuumOff(EC::Context& ctx);
        EC::TaskResult HandleMoveSafeZAfterPlace(EC::Context& ctx);
        EC::TaskResult HandleCheckRepeat(EC::Context& ctx);
        EC::TaskResult HandleComplete(EC::Context& ctx);

        // 유틸리티 메서드
        void LogStep(const std::string& message);

        // 멤버 변수
        Load2Parts* m_parts;
        int m_repeatCount;
        int m_currentIteration;
        int m_successCount;
        long m_moveTimeoutMs;

        // 위치 파라미터
        double m_pickX, m_pickZ;
        double m_placeX, m_placeZ;
        double m_safeZ;

        // 액추에이터 인덱스
        int m_vacuumIndex;

        // 상태
        std::string m_lastError;
    };

    using ThreadLoad2Ptr = std::shared_ptr<ThreadLoad2>;

} // namespace OperationThread
