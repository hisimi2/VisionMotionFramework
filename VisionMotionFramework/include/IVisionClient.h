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

	enum VisionCommand : int
	{
		SetCok = 0,
		InspReady = 1,
		Measure = 2,
		DeviceCheck = 3,
		Light = 4,
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

        // 비동기 명령 전송
		virtual bool RequestSetCokAsync(const StringMap& params)        = 0;
		virtual bool RequestInspReadyAsync(const StringMap& params)     = 0;
		virtual bool RequestMeasureAsync(const StringMap& params)       = 0;
		virtual bool RequestDeviceCheckAsync(const StringMap& params)   = 0;
		virtual bool RequestLightAsync(const StringMap& params)         = 0;

        // 수신 데이터 조회
        virtual DataMap GetLatestData(VisionCommand type) const            = 0;
        virtual void ClearLatestData(VisionCommand type)                   = 0;
        virtual bool IsValid(VisionCommand type) const                     = 0;
        virtual bool HasReceived(VisionCommand type) const                 = 0;

        // 수신 스레드 및 콜백
        virtual void InitializeRecvThread()         = 0;
		virtual void OnSetCok(ByteArray body)       = 0;
		virtual void OnInspReady(ByteArray body)    = 0;
		virtual void OnMeasure(ByteArray body)      = 0;
		virtual void OnDeviceCheck(ByteArray body)  = 0;
		virtual void OnLight(ByteArray body)        = 0;
    };
} // namespace VMF

