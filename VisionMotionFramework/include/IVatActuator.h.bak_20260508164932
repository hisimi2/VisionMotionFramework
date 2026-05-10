#pragma once
#include "DVH_VAT_API.h"

#include <vector>
#include "Types.h"

namespace DVH_VAT
{
	enum PitchType
	{
		Fixed,      // 비가변
		Variable    // 가변
	};

	enum PitchStatus
	{
		Narrow,
		Wide
	};

	enum ActError
    {
        // ----- 정상 / 진행 상태 -----
        ActOk = 0,                 // 정상 완료
        ActWait,                   // 동작 진행 중 (Motion not done)

        // ----- 일반 실패 -----
        ActFail,                   // 일반 실패 (fallback)

        // ----- 입력 / 조건 오류 -----
        ActInvalidParam,           // 파라미터 오류 (pos size, null 등)
        ActOutOfRange,             // Limit 초과 (X/Y/Z/Table)
        ActSpeedLimit,             // 속도 제한 초과

        // ----- 상태 문제 -----
        ActNotReady,               // 초기화 안됨 / 준비 안됨
        ActBusy,                   // 다른 동작 수행 중
        ActTimeout,                // 동작 시간 초과

        // ----- 안전 관련 -----
        ActUnsafe,                 // 일반 안전 조건 불만족
        ActZNotSafe,               // Z축 안전 위치 아님
        ActInterlock,              // 인터락 걸림
        ActCollisionRisk,          // 충돌 위험

        // ----- 하드웨어 관련 -----
        ActNoParts,                // m_parts 없음
        ActHwError,                // 장비 내부 오류
        ActCommError,              // 통신 실패

        // ----- Motion 관련 -----
        ActMoveFail,               // Move 명령 실패
        ActNotInPosition,          // 위치 도달 실패 (tolerance 초과)

        // ----- 상태 불일치 -----
        ActStateMismatch           // Pitch 등 상태 불일치
    };

	class DVH_VAT_API IVatActuator
	{
	public:
		IVatActuator() = default;
		virtual ~IVatActuator() = default;

		virtual PitchType GetPitchType() = 0;
		virtual ActError IsReadyToMove() = 0;
		virtual ActError MoveZ(double targetZ) = 0;
		virtual ActError Move(std::vector<double> pos, PitchStatus action) = 0;
		virtual ActError isMoveZ(double targetZ) = 0;
		virtual ActError isMove(std::vector<double> pos, PitchStatus action) = 0;
		virtual std::vector<double> getPosition() = 0;
		virtual std::vector<double> getPulse() = 0;
		virtual int SetLightState(int camIndex, bool on) = 0;
		virtual int GetLightState(int camIndex, bool& outOn) = 0;
	};

} // namespace DVH_VAT
