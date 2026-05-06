#pragma once

#include "IVatActuator.h"
#include "DVH_VAT/Load1/Load1Parts.h"

namespace VAT_LOAD1
{
	/// <summary>
    /// RobotUnit의 구성 요소들을 IVatActuator 인터페이스로 래핑하는 어댑터 클래스입니다.
	/// </summary>
	class VatAdapterLoad1 : public DVH_VAT::IVatActuator
	{
        Load1Parts* m_parts;
	public:
		explicit VatAdapterLoad1(Load1Parts* parts);
		virtual ~VatAdapterLoad1();

		DVH_VAT::PitchType GetPitchType() override;
		DVH_VAT::ActError IsReadyToMove() override;
		DVH_VAT::ActError MoveZ(double targetZ) override;
		DVH_VAT::ActError Move(std::vector<double> pos, DVH_VAT::PitchStatus action) override;
		DVH_VAT::ActError isMoveZ(double targetZ) override;
		DVH_VAT::ActError isMove(std::vector<double> pos, DVH_VAT::PitchStatus action) override;
		std::vector<double> getPosition() override;
		std::vector<double> getPulse() override;
        int SetLightState(int camIndex, bool on) override;
        int GetLightState(int camIndex, bool& outOn) override;
	};
} // namespace VAT_LOAD1
