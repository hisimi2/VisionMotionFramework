#pragma once

#include "IActuator.h"
#include "Actuators/Load1Parts.h"

namespace VMF_Load1
{
	/// <summary>
    /// RobotUnit의 구성 요소들을 IVatActuator 인터페이스로 래핑하는 어댑터 클래스입니다.
	/// </summary>
	class VatAdapterLoad1 : public VMF::IActuator
	{
        Load1Parts* m_parts;
	public:
		explicit VatAdapterLoad1(Load1Parts* parts);
		virtual ~VatAdapterLoad1();

		VMF::PitchType GetPitchType() override;
		VMF::ActError IsReadyToMove() override;
		VMF::ActError MoveZ(double targetZ) override;
		VMF::ActError Move(std::vector<double> pos, VMF::PitchStatus action) override;
		VMF::ActError isMoveZ(double targetZ) override;
		VMF::ActError isMove(std::vector<double> pos, VMF::PitchStatus action) override;
std::vector<double> GetPosition() override;
		std::vector<double> GetPulse() override;
        int SetLightState(int camIndex, bool on) override;
        int GetLightState(int camIndex, bool& outOn) override;
	};
} // namespace VMF_Load1
