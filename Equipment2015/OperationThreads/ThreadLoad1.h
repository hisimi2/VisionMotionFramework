#pragma once

#include "Actuators/Load1Parts.h"
#include <memory>
#include "TaskBase.h"

namespace OperationThread
{
    /// <summary>
    /// Load1 Pick & Place 시퀀스 상태머신
    /// EC::TaskBase 기반 상태 기계 구현
    /// 
    /// 작업 흐름:
    /// 1. Rail Open
    /// 2. Move to Pick Position (XY)
    /// 3. Preciser Down
    /// 4. Move to Pick Z
    /// 5. Clamp
    /// 6. Vacuum On
    /// 7. Move to Safe Z
    /// 8. Move to Place Position (XY)
    /// 9. Move to Place Z
    /// 10. Release
    /// 11. Blow
    /// 12. Move to Safe Z
    /// 13. 반복 또는 완료
    /// </summary>
    class ThreadLoad1 : public EC::TaskBase
    {
    public:
        /// <summary>
        /// Pick & Place 작업 상태
        /// </summary>
        enum Substep
        {
            RailOpen = 0,               // 0. 레일 열기
            MovePickPositionXY,         // 1. Pick 위치로 XY 이동
            PreciserDown,               // 2. 정밀도 하강
            MovePickPositionZ,          // 3. Pick 위치로 Z 하강
            ClampPick,                  // 4. 크램프로 물품 잡기
            VacuumOn,                   // 5. 진공 활성화
            MoveSafeZAfterPick,         // 6. 안전 높이로 Z 상승
            MovePlacePositionXY,        // 7. Place 위치로 XY 이동
            MovePlacePositionZ,         // 8. Place 위치로 Z 하강
            ReleasePlace,               // 9. 크램프 해제
            BlowOn,                     // 10. 공기 분사
            MoveSafeZAfterPlace,        // 11. 안전 높이로 Z 상승
            CheckRepeat,                // 12. 반복 여부 확인
            Complete                    // 13. 작업 완료
        };

        /// <summary>
        /// Load1 Pick & Place 시퀀스 생성자
        /// </summary>
        /// <param name="parts">Load1Parts 객체</param>
        /// <param name="repeatCount">반복 횟수 (0 = 무한 반복)</param>
        ThreadLoad1(LPVOID parts, int repeatCount = 0);

        ~ThreadLoad1() override;

        std::string GetName() const override { return "ThreadLoad1"; }

        /// <summary>
        /// Pick 위치 설정
        /// </summary>
        void SetPickPosition(double x, double y, double z);

        /// <summary>
        /// Place 위치 설정
        /// </summary>
        void SetPlacePosition(double x, double y, double z);

        /// <summary>
        /// Safe Z 위치 설정
        /// </summary>
        void SetSafeZ(double z);

        /// <summary>
        /// 타임아웃 설정 (밀리초)
        /// </summary>
        void SetMoveTimeout(long timeoutMs);

        /// <summary>
        /// Clamp 인덱스 설정
        /// </summary>
        void SetClampIndex(int index);

        /// <summary>
        /// Vacuum 인덱스 설정
        /// </summary>
        void SetVacuumIndex(int index);

        /// <summary>
        /// 현재 단계 반환
        /// </summary>
        Substep GetCurrentStep() const { return static_cast<Substep>(GetState()); }

    protected:
        void OnInitialize(EC::Context& ctx) override;
        EC::TaskResult OnPoll(EC::Context& ctx) override;

    private:
        // 단계 처리 함수들 (TaskResult 반환)
        EC::TaskResult HandleRailOpen(EC::Context& ctx);
        EC::TaskResult HandleMovePickPositionXY(EC::Context& ctx);
        EC::TaskResult HandlePreciserDown(EC::Context& ctx);
        EC::TaskResult HandleMovePickPositionZ(EC::Context& ctx);
        EC::TaskResult HandleClampPick(EC::Context& ctx);
        EC::TaskResult HandleVacuumOn(EC::Context& ctx);
        EC::TaskResult HandleMoveSafeZAfterPick(EC::Context& ctx);
        EC::TaskResult HandleMovePlacePositionXY(EC::Context& ctx);
        EC::TaskResult HandleMovePlacePositionZ(EC::Context& ctx);
        EC::TaskResult HandleReleasePlace(EC::Context& ctx);
        EC::TaskResult HandleBlowOn(EC::Context& ctx);
        EC::TaskResult HandleMoveSafeZAfterPlace(EC::Context& ctx);
        EC::TaskResult HandleCheckRepeat(EC::Context& ctx);
        EC::TaskResult HandleComplete(EC::Context& ctx);

        Load1Parts* m_parts;

        // 파라미터
        double m_pickX, m_pickY, m_pickZ;
        double m_placeX, m_placeY, m_placeZ;
        double m_safeZ;
        int m_clampIndex;
        int m_vacuumIndex;
        long m_moveTimeoutMs;

        // 반복 관련
        int m_repeatCount;
        int m_currentIteration;

        // 유틸리티
        void LogStep(const std::string& message);
        bool IsMovementComplete();
    };

    using ThreadLoad1Ptr = std::shared_ptr<ThreadLoad1>;
}
