#pragma once

#include "IVatActuator.h"
#include "DVH_VAT/Load1/Load1Parts.h"

namespace VAT_LOAD1
{
	/// <summary>
    /// RobotUnit의 구성 요소들을 IVatActuator 인터페이스로 래핑하는 어댑터 클래스입니다.
	/// </summary>
	class VatAdapterLoad1 : public VMF::IVatActuator
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
		std::vector<double> getPosition() override;
		std::vector<double> getPulse() override;
        int SetLightState(int camIndex, bool on) override;
        int GetLightState(int camIndex, bool& outOn) override;
	};
} // namespace VAT_LOAD1
