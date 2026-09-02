#pragma once

#include "VMF_API.h"
#include "Types.h"
#include "VisionComm\Controller.h"

#include <vector>
#include <cstdint>
#include <string>

namespace VMF
{
    /// "VMF는 '어떻게(How) 보낼지'만 정의하고, '무엇을(What/Which Keys) 보낼지'는 Equipment 프로젝트가 결정

    using ByteArray = std::vector<uint8_t>;

    class IResultSink;

	/// <summary>
	/// Vision 명령 유형 (3가지 핵심 인터페이스로 정리)
	/// 
	/// 분류 기준:
	///   SetInformation  → 장비/검사 정보 설정 및 전달 (SET_COK, Orientation, Piggyback 등)
	///   RequestResult   → 검사 결과 요청 및 수신 (PLVI: S107/F6, VAT: S107/F9, 6Side: S107/F2)
	///   Measure         → 실제 검사 실행 명령 (PLVI: S107/F5, VAT: S107/F9, 6Side: S107/F1)
	/// </summary>
	enum VisionCommand : int
	{
		SetInformation = 0,
		RequestResult = 1,
		Measure = 2,
		Unknown = 200
   };


    struct VisionConnectionConfig
    {
        std::string type;
        std::string address;
        int port;
        int timeoutMs;

        VisionConnectionConfig()
            : type(""), address(""), port(0), timeoutMs(0)
        {
        }

        VisionConnectionConfig(std::string address, int port, int timeoutMs)
            : type(""), address(std::move(address)), port(port), timeoutMs(timeoutMs) 
        {
        }
    };

    class VMF_API IVisionClient
    {
    public:
        using DataMap = StringMap;
        virtual ~IVisionClient() = default;

        // 연결 관리
        virtual VC::Status Initialize(const VisionConnectionConfig& config) = 0;

        /// <summary>
        /// 외부에서 공유되는 Controller를 사용하여 초기화합니다.
        /// VisionConnectionManager를 통해 공유 소켓 연결을 사용할 때 호출됩니다.
        /// 기본 구현은 Initialize(config)를 호출합니다.
        /// </summary>
        virtual VC::Status InitializeWithSharedController(
            std::shared_ptr<VC::Controller> sharedCtrl,
            const VisionConnectionConfig& config)
        {
            // 기본 구현: sharedCtrl 무시하고 config로 직접 초기화
            return Initialize(config);
        }

        virtual void Disconnect()           = 0;
        virtual bool IsConnected() const    = 0;

        // ─────────────────────────────────────────────────────────────
        // 3가지 핵심 인터페이스 (비동기 명령 전송)
        // ─────────────────────────────────────────────────────────────

        /// <summary>
        /// 장비/검사 정보 설정 및 전달
        /// </summary>
        /// <details>
        /// 포함 기능:
        ///   - SET_COK (S107/F11): COK 파라미터 및 PKG 정보 설정
        ///   - Orientation Check 준비 (S107/F7): A1 index mark 방향/위치 검사 요청
        ///   - Piggyback 위치 전달 (S107/F9): 검사 전 위치 정보 전달
        ///   - Light 제어 등 정보성 명령
        /// 
        /// PLVI에서는 현재 미사용 (필요시 확장 가능)
        /// 
        /// 파라미터 예시 (장비별):
        ///   PLVI: PLVI_POSITION, PKG_NAME, CTRAY_X, CTRAY_Y, DEVICE_INFO, DATA_ID
        ///   VAT:  RECIPE_NAME, COK_TYPE, CAMERA_ID 등
        /// </details>
        /// <param name="params">장비별 파라미터 (StringMap)</param>
        /// <returns>송신 성공 여부</returns>
        virtual bool SetInformationAsync(const StringMap& params) = 0;

        /// <summary>
        /// 검사 결과 요청
        /// </summary>
        /// <details>
        /// 포함 기능:
        ///   - PLVI: S107/F6 결과 수신 (REQ_RESULT)
        ///   - VAT: S107/F9 측정 결과 수신
        ///   - 6Side: S107/F2 검사 결과 수신
        /// 
        /// PLVI의 경우 params는 선택적이며, nDataID만 전송할 수 있습니다.
        /// </details>
        /// <param name="params">결과 요청에 필요한 파라미터 (장비별)</param>
        /// <returns>송신 성공 여부</returns>
        virtual bool RequestResultAsync(const StringMap& params) = 0;

        /// <summary>
        /// 실제 검사 실행 명령
        /// </summary>
        /// <details>
        /// 포함 기능:
        ///   - PLVI: S107/F5 REQ_MEASURE (검사 시작 요청)
        ///   - VAT: S107/F9 REQ_MEASURE (VAT 측정)
        ///   - 6Side: S107/F1 CMD_6_SIDE_INSPECTION_MEASURE
        /// 
        /// 파라미터 예시 (PLVI 기준):
        ///   PLVI_POSITION, PKG_NAME, CTRAY_X, CTRAY_Y, DEVICE_INFO, DATA_ID
        /// </details>
        /// <param name="params">검사에 필요한 파라미터 (장비별)</param>
        /// <returns>송신 성공 여부</returns>
        virtual bool MeasureAsync(const StringMap& params) = 0;

        // ─────────────────────────────────────────────────────────────
        // 수신 데이터 조회
        // ─────────────────────────────────────────────────────────────
        virtual DataMap GetLatestData(VisionCommand type) const            = 0;
        virtual void ClearLatestData(VisionCommand type)                   = 0;
        virtual bool IsValid(VisionCommand type) const                     = 0;
        virtual bool HasReceived(VisionCommand type) const                 = 0;

        // ─────────────────────────────────────────────────────────────
        // 수신 스레드 및 콜백 (3가지로 통합)
        // ─────────────────────────────────────────────────────────────
        virtual void InitializeRecvThread()         = 0;
        virtual void OnSetInformation(ByteArray body) = 0;  // SetInformation 결과 수신
        virtual void OnRequestResult(ByteArray body)  = 0;  // RequestResult 수신
        virtual void OnMeasure(ByteArray body)        = 0;  // Measure 결과 수신
    };
} // namespace VMF


