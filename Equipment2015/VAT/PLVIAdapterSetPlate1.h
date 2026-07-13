#pragma once
#include "IActuator.h"
#include "Actuators\SetPlate1Parts.h"

namespace VMF
{
	class PLVIAdapterSetPlate1 : public VMF::IActuator
	{
		SetPlate1Parts* m_parts;

		// 트리거 상태
		bool   m_triggerEnabled;
		double m_triggerIntervalMm;

		// 위치별 트리거 파라미터
		// 실제 HW 연결 시 생성자 인자 또는 Configure()로 주입
		double m_scanDistMm;    // 촬영 구간 거리 (SetPlate1=315, Shuttle=532)
		double m_accelDistMm;   // 가속 여유 거리 (공통적으로 10사용)

	public:
		explicit PLVIAdapterSetPlate1(SetPlate1Parts* parts);
		virtual ~PLVIAdapterSetPlate1();

		VMF::PitchType GetPitchType()  override;
		VMF::ActError  IsReadyToMove() override;

		VMF::ActError  MoveZ(double targetZ)          override;
		VMF::ActError  isMoveZ(double targetZ)         override;
		VMF::ActError  Move(VMF::MotionCommand& cmd) override;
		VMF::ActError  isMove(VMF::MotionCommand& cmd) override;
		VMF::ActError  Stop()                        override;


		int SetLightState(int camIndex, bool on)      override;
		int GetLightState(int camIndex, bool& outOn)  override;

		VMF::ActError SetTriggerState(bool enable, double intervalMm)          override;
		VMF::ActError GetTriggerState(bool& outEnabled, double& outIntervalMm) override;

		// PLVI 전용 헬퍼 
		VMF::ActError ClampSetplate(int idx, bool clamp);
		VMF::ActError IsSetplateClamped(int idx) const;
		VMF::ActError LiftSetplate(int idx, bool up);
		VMF::ActError IsSetplateLift(int idx) const;
	};

} // namespace VMF_PLVI
