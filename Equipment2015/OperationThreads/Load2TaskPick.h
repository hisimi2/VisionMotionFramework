#pragma once

namespace EC
{
    class TaskBase;
    class Context;
    class Load2Parts;

    /// <summary>
    /// Load2 Pick 시퀀스 상태머신
    /// TaskBase 기반 상태 기계 구현
    /// 
    /// Load2는 Load1과 달리 XY축 대신 X축 + XPitch를 사용합니다.
    /// 
    /// 작업 흐름:
    /// 1. Rail Open
    /// 2. Move to Pick Position (X + XPitch)
    /// 3. Preciser Down
    /// 4. Move to Pick Z
    /// 5. Vacuum On
    /// 6. Move to Safe Z
    /// </summary>
    class Load2TaskPick : public TaskBase
    {
    public:
        /// <summary>
        /// Pick 작업 상태
        /// </summary>
        enum Substep
        {
            RailOpen = 0,               // 0. 레일 열기
            MovePickPositionX,          // 1. Pick 위치로 X 이동
            PreciserDown,               // 2. 정밀도 하강
            MovePickPositionZ,          // 3. Pick 위치로 Z 하강
            VacuumOn,                   // 4. 진공 활성화
            MoveSafeZAfterPick,         // 5. 안전 높이로 Z 상승
            Complete
        };

        /// <summary>
        /// Load2 Pick 시퀀스 생성자
        /// </summary>
        /// <param name="parts">Load2Parts 객체 (shared_ptr)</param>
        Load2TaskPick(std::shared_ptr<Load2Parts> parts);

        ~Load2TaskPick() override;

        /// <summary>
        /// Pick 위치 설정
        /// </summary>
        void SetPickPosition(double x, double z);

        /// <summary>
        /// XPitch 위치 설정 (Width)
        /// </summary>
        void SetXPitchWidth(double width);

        /// <summary>
        /// Safe Z 위치 설정
        /// </summary>
        void SetSafeZ(double z);

        /// <summary>
        /// 타임아웃 설정 (밀리초)
        /// </summary>
        void SetMoveTimeout(long timeoutMs);

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
        TaskResult OnPoll(Context& ctx) override;

    private:
        // 단계 처리 함수들
        TaskResult HandleRailOpen(Context& ctx);
        TaskResult HandleMovePickPositionX(Context& ctx);
        TaskResult HandlePreciserDown(Context& ctx);
        TaskResult HandleMovePickPositionZ(Context& ctx);
        TaskResult HandleVacuumOn(Context& ctx);
        TaskResult HandleMoveSafeZAfterPick(Context& ctx);
        TaskResult HandleComplete(Context& ctx);

        std::shared_ptr<Load2Parts> m_parts;

        // 파라미터
        double m_pickX, m_pickZ;
        double m_xPitchWidth;
        double m_safeZ;
        int m_vacuumIndex;
        long m_moveTimeoutMs;

        // 유틸리티
    };

    using Load2TaskPickPtr = std::shared_ptr<Load2TaskPick>;
}
