#pragma once

#include "IActuator.h"

namespace VMF
{
    /// IActuator 구현체 (HW Parts Adapter) – 실제 장비의 Load1Parts 객체를 IActuator 인터페이스로 연결
    class LoadPPAdapter : public VMF::IActuator
    {
    public:
        // 생성자는 실제 장비 Parts 객체를 받아야 함
        explicit LoadPPAdapter();
        virtual ~LoadPPAdapter();

        // 실제 장비의 PitchType을 반환
        VMF::PitchType GetPitchType()       override;
        VMF::ActError  IsReadyToMove()      override;

        VMF::ActError  MoveZ(double targetZ)          override;
        VMF::ActError  Move(VMF::MotionCommand& cmd) override;
        VMF::ActError  isMoveZ(double targetZ)         override;
        VMF::ActError  isMove(VMF::MotionCommand& cmd)override;
        VMF::ActError  Stop()                          override;

        std::vector<double> getPosition() override;
        std::vector<double> getPulse()    override;

        // int 반환: IActuator 인터페이스와 일치
        int  SetLightState(int cameraId, bool on) override;
        int  GetLightState(int camIndex, bool& outOn) override;

        VMF::ActError SetLaserState(int laserChannel, bool on) override;
        VMF::ActError GetLaserState(int laserChannel, bool& outOn) override;

        VMF::ActError SetTriggerState(bool enable, double intervalMm) override;
        VMF::ActError GetTriggerState(bool& outEnabled, double& outIntervalMm) override;
    };
} // namespace VMF_PLUGIN
