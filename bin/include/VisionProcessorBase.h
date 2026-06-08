#pragma once

#include <memory>
#include <vector>
#include <mutex> 

#include "VMF_API.h"
#include "IVisionProcessor.h"
#include "IResultSink.h"
#include "Controller.h"

namespace VMF
{
    class VMF_API VisionProcessorBase : public IVisionEventHandler
    {
    public:
        using DataMap = StringMap;

        VisionProcessorBase();
        ~VisionProcessorBase() override;

        void Start();
        void Stop();
        
        // 상속된 가상 함수 (override 명시)
        void InitializeRecvThread() override;

        void RunLoop();
        virtual void Process(); // 상속 구조에 따라 override 여부가 결정되나, 자식 클래스에서 오버라이드할 기본 제공 가상 함수로 보입니다.

        VC::Status Initialize(const VisionConnectionConfig& config) override;
        void Disconnect() override;
        bool IsConnected() const override;

        DataMap GetLatestData(VatCommand type) const override;
        void SetLatestData(VatCommand type, const DataMap& data);
        void ClearLatestData(VatCommand type) override;
        bool IsValid(VatCommand type) const override;
        bool HasReceived(VatCommand type) const override;

void SetReceived(VatCommand type, bool received);
        void ClearReceived(VatCommand type);

        /// <summary>
        /// 비전 프로세서의 실행 결과를 수신할 IResultSink를 등록합니다.
        /// </summary>
        /// <param name="sink">결과를 전달받을 싱크 객체 포인터</param>
        void SetResultSink(IResultSink* sink);

        void StartProcessThread();
        void StopProcessThread();

    protected:
        VC::Controller m_ctrl;
        std::mutex m_mutex;

    private:
        struct Impl;
        // 이미 스마트 포인터(std::unique_ptr)가 사용되고 있음
        std::unique_ptr<Impl> m_impl;

        void SendResultToSink(int requestId, const std::vector<std::string>& results);
        void PacketLoop();
    };
} // namespace VMF

