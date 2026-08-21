#pragma once
#include "IActuator.h"
#include "Actuators\SetPlate1Parts.h"
#include <vector>

namespace VMF
{
    class PLVIAdapterSetPlate1 : public VMF::IActuator
    {
        SetPlate1Parts* m_parts;

        // 트리거 상태
        bool   m_triggerEnabled;
        double m_triggerIntervalMm;

        // 위치별 트리거 파라미터
        double m_scanDistMm;    // 촬영 구간 거리 (SetPlate1=315, Shuttle=532)
        double m_accelDistMm;   // 가속 여유 거리 (공통적으로 10사용)

        // VisionPosition 기반 위치 관리
        VMF::VisionPosition m_startPos;      // 측정 시작 위치
        VMF::VisionPosition m_scanEndPos;    // 측정 종료 위치
        VMF::VisionPosition m_safeZPos;      // 안전 Z 위치
        VMF::VisionPosition m_homePos;       // 홈 위치

    public:
        explicit PLVIAdapterSetPlate1(SetPlate1Parts* parts);
        virtual ~PLVIAdapterSetPlate1();

        VMF::PitchType GetPitchType()  override;
        VMF::ActError  IsReadyToMove() override;

        // ── Z축 이동 (VisionPosition 기반) ──
        VMF::ActError MoveZ(const VMF::VisionPosition& target) override;
        VMF::ActError isMoveZ(const VMF::VisionPosition& target) override;

        // ── 일반 축 이동 ──
        VMF::ActError Move(VMF::MotionCommand& cmd) override;
        VMF::ActError isMove(VMF::MotionCommand& cmd) override;
        VMF::ActError Stop() override;

        // ── 측정 위치 이동 (VisionPosition 기반) ──
        VMF::ActError MoveToMeasurementPosition(const VMF::VisionPosition& target) override;
        VMF::ActError IsAtMeasurementPosition(const VMF::VisionPosition& target) override;

        // ── 홈 위치 이동 (VisionPosition 기반) ──
        VMF::ActError MoveToHomePosition(const VMF::VisionPosition& target) override;
        VMF::ActError IsAtHomePosition(const VMF::VisionPosition& target) override;

        // ── Z축 이동/확인 (VisionPosition 기반) ──
        VMF::ActError MoveToZ(const VMF::VisionPosition& target) override;
        VMF::ActError IsAtZ(const VMF::VisionPosition& target) override;

        // ── 안전 Z 이동/확인 (VisionPosition 기반) ──
        VMF::ActError MoveToZSafe(const VMF::VisionPosition& target) override;
        VMF::ActError IsAtZSafe(const VMF::VisionPosition& target) override;

        // ── 조명/레이저 제어 ──
        VMF::ActError SetLaserState(int laserChannel, bool on, int laserIndex = 0) override;
        VMF::ActError GetLaserState(int laserChannel, bool& outOn, int laserIndex = 0) override;
        VMF::ActError SetLightState(int camIndex, bool on, int lightIndex = 0) override;
        VMF::ActError GetLightState(int camIndex, bool& outOn, int lightIndex = 0) override;

        // ── 트리거 제어 ──
        VMF::ActError SetTriggerState(bool enable, double intervalMm, int triggerIndex = 0) override;
        VMF::ActError GetTriggerState(bool& outEnabled, double& outIntervalMm, int triggerIndex = 0) override;

        // ── 준비/완료 동작 (VisionPosition 기반) ──
        VMF::ActError PrepareForInspection(const VMF::VisionPosition& target) override;
        VMF::ActError IsAtPrepareForInspection(const VMF::VisionPosition& target) override;
        VMF::ActError CompleteInspection(const VMF::VisionPosition& target) override;
        VMF::ActError IsAtCompleteInspection(const VMF::VisionPosition& target) override;

        // ── PLVI 전용 헬퍼 ──
        VMF::ActError ClampSetplate(int idx, bool clamp);
        VMF::ActError IsSetplateClamped(int idx) const;
        VMF::ActError LiftSetplate(int idx, bool up);
        VMF::ActError IsSetplateLift(int idx) const;

        // ── 위치 파라미터 설정 ──
        void SetPositionParams(const std::vector<VMF::VisionPosition>& positions);
    };
} // namespace VMF
