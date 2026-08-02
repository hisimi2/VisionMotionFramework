#pragma once
#include "VMF_API.h"
#include "Types.h"

namespace VMF
{

    enum class ActError { // C++11 enum class로 타입 안전성 강화
        ActOk = 0,
        ActWait, // 동작 진행 중
        ActFail, // 일반 실패
        ActInvalidParam, // 파라미터 오류
        ActOutOfRange, // Limit 초과
        ActSpeedLimit, // 속도 제한 초과
        ActNotReady, // 초기화 안됨
        ActBusy, // 다른 동작 수행 중
        ActTimeout, // 동작 시간 초과
        ActUnsafe, // 안전 조건 미충족
        ActZNotSafe, // Z축 안전 위치 아님
        ActInterlock, // 인터락 걸림
        ActCollisionRisk, // 충돌 위험
        ActNoParts, // m_parts 없음
        ActHwError, // 장비 내부 오류
        ActCommError, // 통신 실패
        ActMoveFail, // Move 명령 실패
        ActNotInPosition, // 위치 도달 실패
        ActStateMismatch // 상태 불일치
    };

    class VMF_API IActuator {
    public:
        IActuator() = default;
        virtual ~IActuator() = default;

        // ── 측정 위치 이동 (비동기 지원) ──
        // Z‑축을 제외한 모든 축(X, Y, 테이블 등)을 측정 위치로 이동시킵니다.
        virtual ActError MoveToMeasurementPosition(const VisionPosition& target) = 0;

        // ── 측정 위치 도달 여부 확인 ──
        // 현재 위치가 측정 위치에 도달했는지 확인합니다.
        virtual ActError IsAtMeasurementPosition(const VisionPosition& target) = 0;

        // ── 홈 위치 이동 및 확인 ──
        // Vision 측정을 완료한 후, Z축을 제외한 나머지 축(X, Y 등)을 회피 또는 대기 위치로 이동시킵니다.
        virtual ActError MoveToHomePosition() = 0;
        virtual ActError IsAtHomePosition() = 0;

        // ── Z축 이동 및 확인 ──
        virtual ActError MoveToZ(double targetZ) = 0;
        virtual ActError IsAtZ(double targetZ) = 0;

        // ── 조명/레이저 제어 ──
        virtual ActError SetLaserState(int laserChannel, bool on) = 0;
        virtual ActError GetLaserState(int laserChannel, bool& outOn) = 0;
        virtual ActError SetLightState(int camIndex, bool on) = 0;
        virtual ActError GetLightState(int camIndex, bool& outOn) = 0;

        // ── 트리거 제어 ──
        virtual ActError SetTriggerState(bool enable, double intervalMm) = 0;
        virtual ActError GetTriggerState(bool& outEnabled, double& outIntervalMm) = 0;

        // ── 충돌 방지 및 안전 이동 ──
        // MoveMeasurementLocation() 수행 시 충돌을 방지하기 위해 Z축 또는 실린더를 회피 위치로 이동시킵니다.
        virtual ActError MoveToZSafe() = 0;
        virtual ActError IsAtZSafe(double targetZ) = 0;

        /// 측정 준비 동작: 실린더 클램프 + Gripper Safety 위치로 이동
        /// Robot 구성에 따라 내부적으로 모터/실린더를 조합하여 수행
        virtual ActError PrepareForInspection() = 0;
        /// 측정 준비 완료 여부 확인
        virtual ActError IsAtPrepareForInspection() = 0;

        /// 측정 완료 동작: 실린더 언클램프 + 후진
        /// Robot 구성에 따라 내부적으로 모터/실린더를 조합하여 수행
        virtual ActError CompleteInspection() = 0;
        /// 측정 완료 여부 확인
        virtual ActError IsAtCompleteInspection() = 0;
    };
} // namespace VMF
