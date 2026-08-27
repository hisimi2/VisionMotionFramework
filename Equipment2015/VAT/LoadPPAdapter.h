#pragma once

#include "VisionMotionFramework\IActuator.h"

class Load1Parts;

namespace VMF
{
    class LoadPPAdapter : public IActuator
    {
        Load1Parts* m_parts;

    public:
        explicit LoadPPAdapter(Load1Parts* parts);
        virtual ~LoadPPAdapter();

        // ── 측정 위치 이동 (VisionPosition 기반) ──
        ActError MoveToMeasurementPosition(const VisionPosition& target) override;
        ActError IsAtMeasurementPosition(const VisionPosition& target) override;

        // ── 홈 위치 이동 (VisionPosition 기반) ──
        ActError MoveToHomePosition(const VisionPosition& target) override;
        ActError IsAtHomePosition(const VisionPosition& target) override;

        // ── Z축 이동/확인 (VisionPosition 기반) ──
        ActError MoveToZ(const VisionPosition& target) override;
        ActError IsAtZ(const VisionPosition& target) override;

        // ── 안전 Z 이동/확인 (VisionPosition 기반) ──
        ActError MoveToZSafe(const VisionPosition& target) override;
        ActError IsAtZSafe(const VisionPosition& target) override;

        // ── 조명/레이저 제어 ──
        ActError SetLaserState(int laserChannel, bool on, int laserIndex = 0) override;
        ActError GetLaserState(int laserChannel, bool& outOn, int laserIndex = 0) override;
        ActError SetLightState(int camIndex, bool on, int lightIndex = 0) override;
        ActError GetLightState(int camIndex, bool& outOn, int lightIndex = 0) override;

        // ── 트리거 제어 ──
        ActError SetTriggerState(bool enable, double intervalMm, int triggerIndex = 0) override;
        ActError GetTriggerState(bool& outEnabled, double& outIntervalMm, int triggerIndex = 0) override;

        // ── 준비/완료 동작 (VisionPosition 기반) ──
        ActError PrepareForInspection(const VisionPosition& target) override;
        ActError IsAtPrepareForInspection(const VisionPosition& target) override;
        ActError CompleteInspection(const VisionPosition& target) override;
        ActError IsAtCompleteInspection(const VisionPosition& target) override;
    };

}

