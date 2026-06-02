#pragma once
#include "Actuators/Load2Parts.h"
#include "TaskBase.h"

namespace OperationThread
{
    using namespace EC;

    /// <summary>
    /// Load2 Place 시퀀스 상태머신
    /// EC::TaskBase 기반 상태 기계 구현
    /// 
    /// 작업 흐름:
    /// 1. Move to Place Position (X)
    /// 2. Pusher Forward
    /// 3. Move to Place Z
    /// 4. Blow On (Vacuum 해제)
    /// 5. Pusher Backward
    /// 6. Move to Safe Z
    /// </summary>
    class Load2TaskPlace : public TaskBase
    {
    public:
        /// <summary>
        /// Place 작업 상태
        /// </summary>
        enum Substep
        {
            MovePlacePositionX = 0,     // 0. Place 위치로 X 이동
            PusherForward,              // 1. 푸셔 전진
            MovePlacePositionZ,         // 2. Place 위치로 Z 하강
            BlowOn,                     // 3. 공기 분사 (Vacuum 해제)
            PusherBackward,             // 4. 푸셔 후진
            MoveSafeZAfterPlace,        // 5. 안전 높이로 Z 상승
            Complete
        };

        /// <summary>
        /// Load2 Place 시퀀스 생성자
        /// </summary>
        /// <param name="parts">Load2Parts 객체 (shared_ptr)</param>
        Load2TaskPlace(std::shared_ptr<Load2Parts> parts);

        ~Load2TaskPlace() override;

        /// <summary>
        /// Place 위치 설정
        /// </summary>
        void SetPlacePosition(double x, double z);

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
        Substep GetCurrentStep() const { return static_cast<Substep>(GetState()); }

    protected:
        void OnInitialize(Context& ctx) override;
        EC::TaskResult OnPoll(Context& ctx) override;

    private:
        // 단계 처리 함수들
        TaskResult HandleMovePlacePositionX(Context& ctx);
        TaskResult HandlePusherForward(Context& ctx);
        TaskResult HandleMovePlacePositionZ(Context& ctx);
        TaskResult HandleBlowOn(Context& ctx);
        TaskResult HandlePusherBackward(Context& ctx);
        TaskResult HandleMoveSafeZAfterPlace(Context& ctx);

        std::shared_ptr<Load2Parts> m_parts;

        // 파라미터
        double m_placeX, m_placeZ;
        double m_safeZ;
        int m_vacuumIndex;
        long m_moveTimeoutMs;

        // 유틸리티
        void LogStep(const std::string& message);
    };

    using Load2TaskPlacePtr = std::shared_ptr<Load2TaskPlace>;
}