#pragma once
#include "IActuator.h"

namespace VMF_Sample
{
	/// <summary>
	/// [Sample] IActuator 구현체 (HW Parts Adapter)
	/// 실제 장비의 Load1Parts/IActuator를 VMF IActuator 인터페이스로 연결
	/// 
	/// !!! 수정 가이드 !!!
	/// 1. 생성자: 장비의 실제 Parts 객체(예: Load1Parts*)를 받도록 수정
	/// 2. 축 이름 (X/Y/Z/TABLE1/TABLE2): 장비의 실제 축 매핑으로 변경
	/// 3. GetPitchType(): 장비의 실제 Pitch 타입 반환 로직으로 변경
	/// 4. 각 Move/isMove 구현: 장비의 실제 모션 제어 방식으로 변경
	/// 5. IsReadyToMove(): 장비의 안전 조건(도어, 압력, Gripper 등)을 모두 검사하도록 수정
	/// </summary>
	class SampleActuatorAdapter : public VMF::IActuator
	{
	public:
		// !!! 수정 필요: 생성자는 장비의 실제 Parts 객체를 받도록 변경 !!!
		// 예: explicit SampleActuatorAdapter(Load1Parts* parts);
		explicit SampleActuatorAdapter();
		virtual ~SampleActuatorAdapter();

		VMF::PitchType GetPitchType()       override;
		VMF::ActError  IsReadyToMove()      override;

		VMF::ActError  MoveZ(double targetZ)          override;
		VMF::ActError  Move(VMF::MotionCommand& cmd) override;
		VMF::ActError  isMoveZ(double targetZ)         override;
		VMF::ActError  isMove(VMF::MotionCommand& cmd)override;

		VMF::ActError  SetLightState(int cameraId, bool on) override;
	};
} // namespace VMF_Sample
