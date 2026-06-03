#pragma once
#include "Actuators/Load1Parts.h"
#include "TaskBase.h"

namespace OperationThread
{
    using namespace EC;

    class Load1TaskPlace : public TaskBase
    {
    public:
        enum Substep
        {
            RailOpen = 0,               // 0. 레일 열기
            MovePlacePositionXY,        // 1. Place 위치로 XY 이동
            MovePlacePositionZ,         // 2. Place 위치로 Z 하강
            ReleasePlace,               // 3. 크램프 해제
            BlowOn,                     // 4. 공기 분사
            MoveSafeZAfterPlace,        // 5. 안전 높이로 Z 상승
            CheckRepeat,                // 6. 반복 여부 확인
            Complete                    // 7. 작업 완료
        };

        /// <summary>
        /// Load1 Pick & Place 시퀀스 생성자
        /// </summary>
        /// <param name="parts">Load1Parts 객체 (shared_ptr)</param>
        Load1TaskPlace(std::shared_ptr<Load1Parts> parts);

        ~Load1TaskPlace() override;

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
        Substep GetCurrentStep() const { return static_cast<Substep>(GetStep()); }

    protected:
        void OnInitialize(Context& ctx) override;
        EC::TaskResult OnPoll(Context& ctx) override;

    private:
        // 단계 처리 함수들 (TaskResult 반환)
        TaskResult HandleRailOpen(Context& ctx);
        TaskResult HandleMovePlacePositionXY(Context& ctx);
        TaskResult HandleMovePlacePositionZ(Context& ctx);
        TaskResult HandleReleasePlace(Context& ctx);
        TaskResult HandleBlowOn(Context& ctx);
        TaskResult HandleMoveSafeZAfterPlace(Context& ctx);
        TaskResult HandleCheckRepeat(Context& ctx);
        TaskResult HandleComplete(Context& ctx);

        std::shared_ptr<Load1Parts> m_parts;  // ✅ shared_ptr

        // 파라미터
        double m_placeX, m_placeY, m_placeZ;
        double m_safeZ;
        int m_clampIndex;
        int m_vacuumIndex;
        long m_moveTimeoutMs;

        // 반복 관련
        int m_repeatCount;
        int m_currentIteration;
    };

    using Load1TaskPlacePtr = std::shared_ptr<Load1TaskPlace>;
}
