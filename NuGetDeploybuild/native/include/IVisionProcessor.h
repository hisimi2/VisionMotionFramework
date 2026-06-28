#pragma once

#include "VMF_API.h"
#include "Controller.h"
#include "Types.h"

#include <vector>
#include <cstdint>
#include <string>

namespace VMF
{
    /// "VMF는 '어떻게(How) 보낼지'만 정의하고, '무엇을(What/Which Keys) 보낼지'는 Equipment 프로젝트가 결정

    using ByteArray = std::vector<uint8_t>;

    class IResultSink;

    using VisionCommand = int;

    namespace VisionCommands
    {
        // [VAT] Vision Auto Teaching (0~99)
        constexpr VisionCommand SetCok = 0;
        constexpr VisionCommand InspReady = 1;
        constexpr VisionCommand Measure = 2;   // VAT Measure
        constexpr VisionCommand DeviceCheck = 3;
        constexpr VisionCommand Light = 4;
        constexpr VisionCommand Unknown = 99;

        // [PLVI] Pocket Leave/Vision Inspection (100~199)
        constexpr VisionCommand PLVI_Measure = 100;
        constexpr VisionCommand PLVI_InspReady = 101;

        // [Barcode] (200~299)
        constexpr VisionCommand Barcode_SetRecipe = 200;
        constexpr VisionCommand Barcode_Measure = 201;

        // [Loading Miss] (300~399)
        constexpr VisionCommand LoadingMiss_Measure = 300;

        // [PCD / Backlash] Motor 정밀도 보정 (400~499)
        constexpr VisionCommand PCD_Measure = 400;
        constexpr VisionCommand Backlash_Measure = 401;

        // [ETX] 디바이스 유무 검사 (500~599)
        constexpr VisionCommand ETX_Measure = 500;

        // [Piggy Back] (600~699)
        constexpr VisionCommand PiggyBack_Measure = 600;

        // [Device Align] (700~799)
        constexpr VisionCommand DeviceAlign_Measure = 700;

        // [OCR] (800~899)
        constexpr VisionCommand OCR_SetRecipe = 800;
        constexpr VisionCommand OCR_Measure = 801;

        // [이물질 검사] Foreign Material Inspection (900~999)
        constexpr VisionCommand ForeignMaterial_Measure = 900;

        // [Chuck Target] (1000~1099)
        constexpr VisionCommand ChuckTarget_Measure = 1000;

        // [Needle Pattern / Needle Pin] (1100~1199)
        constexpr VisionCommand NeedlePattern_Measure = 1100;
        constexpr VisionCommand NeedlePin_Measure = 1101;

        // [Ctray Count] (1200~1299)
        constexpr VisionCommand CtrayCount_Measure = 1200;

        // [Ring Frame Center Align] (1300~1399)
        constexpr VisionCommand RingFrameAlign_Measure = 1300;

        // [Device Pitch Inspection] (1400~1499)
        constexpr VisionCommand DevicePitch_Measure = 1400;

        // [Stage Align] (1500~1599)
        constexpr VisionCommand StageAlign_Measure = 1500;

        // [Flying Vision] — 개발 예정 (1600~1699)
        constexpr VisionCommand FlyingVision_Measure = 1600;

    } // namespace VisionCommands

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

    class VMF_API IVisionProcessor
    {
    public:
        using DataMap = StringMap;
        virtual ~IVisionProcessor() = default;

        // 연결 관리
        virtual VC::Status Initialize(const VisionConnectionConfig& config) = 0;
        
        /// <summary>
        /// 외부에서 공유되는 Controller를 사용하여 초기화합니다.
        /// ConnectionManager를 통해 얻은 Controller를 전달하면,
        /// 동일 서버에 대해 단일 소켓 연결을 공유할 수 있습니다.
        /// 기본 구현은 실패(VisionNotInitialized)를 반환합니다.
        /// </summary>
        virtual VC::Status InitializeWithSharedController(
            std::shared_ptr<VC::Controller> sharedCtrl,
            const VisionConnectionConfig& config)
        {
            // 기본 구현: 지원하지 않음
            (void)sharedCtrl;
            (void)config;
            return VC::VisionNotInitialized;
        }
        
        virtual void Disconnect() = 0;
        virtual bool IsConnected() const = 0;

        // 비동기 명령 전송
        virtual bool RequestAsync(VisionCommand cmd, const StringMap& params) = 0;

        // 수신 데이터 조회
        virtual DataMap GetLatestData(VisionCommand type) const            = 0;
        virtual void ClearLatestData(VisionCommand type)                   = 0;
        virtual bool IsValid(VisionCommand type) const                     = 0;
        virtual bool HasReceived(VisionCommand type) const                 = 0;

        // 수신 스레드 및 콜백
        virtual void InitializeRecvThread()         = 0;
        virtual void OnVisionResponse(VisionCommand cmd, ByteArray body) = 0;
    };
} // namespace VMF

