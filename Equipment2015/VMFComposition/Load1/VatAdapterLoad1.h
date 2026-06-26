#pragma once
#include "IActuator.h"
#include "Actuators/Load1Parts.h"

namespace VMF_Load1
{
	class VatAdapterLoad1 : public VMF::IActuator
	{
		Load1Parts* m_parts;
	public:
		explicit VatAdapterLoad1(Load1Parts* parts);
		virtual ~VatAdapterLoad1();

		VMF::PitchType GetPitchType()       override;
		VMF::ActError  IsReadyToMove()      override;

		VMF::ActError  MoveZ(double targetZ)          override;
		VMF::ActError  Move(VMF::MotionCommand& cmd) override;
		VMF::ActError  isMoveZ(double targetZ)         override;
		VMF::ActError  isMove(VMF::MotionCommand& cmd)override;
		VMF::ActError  Stop()                        override;

		std::vector<double> getPosition() override;
		std::vector<double> getPulse()    override;

		int SetLightState(int camIndex, bool on)           override;
		int GetLightState(int camIndex, bool& outOn)       override;

		VMF::ActError SetLaserState(int ch, bool on)                          override;
		VMF::ActError GetLaserState(int ch, bool& outOn)                      override;
		VMF::ActError SetTriggerState(bool enable, double intervalMm)            override;
		VMF::ActError GetTriggerState(bool& outEnabled, double& outIntervalMm)  override;
	};
}