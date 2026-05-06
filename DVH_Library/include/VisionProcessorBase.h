#pragma once

#include <memory>
#include <vector>
#include <mutex> // boost::mutex 대신 C++ 표준 라이브러리 사용

#include "DVH_VAT_API.h"
#include "IVisionProcessor.h"
#include "IResultSink.h"
#include "VisionController.h"

namespace DVH_VAT
{
    class DVH_VAT_API VisionProcessorBase : public IVisionEventHandler
    {
    public:
        // C++11/14: typedef 대신 using 예약어 활용
        using DataMap = StringMap;

        VisionProcessorBase();
        
        // C++11/14: 가상 소멸자 재정의 명시 (override)
        ~VisionProcessorBase() override;

        void Start();
        void Stop();
        
        // 상속된 가상 함수 (override 명시)
        void InitializeRecvThread() override;

        void RunLoop();
        virtual void Process(); // 상속 구조에 따라 override 여부가 결정되나, 자식 클래스에서 오버라이드할 기본 제공 가상 함수로 보입니다.

        VisionCom::VisionStatus Initialize(const VisionConnectionConfig& config) override;
        void Disconnect() override;
        bool IsConnected() const override;

        DataMap GetLatestData(VatCommand type) const override;
        void SetLatestData(VatCommand type, const DataMap& data);
        void ClearLatestData(VatCommand type) override;
        bool IsValid(VatCommand type) const override;
        bool HasReceived(VatCommand type) const override;

        void SetReceived(VatCommand type, bool received);
        void ClearReceived(VatCommand type);

        void StartProcessThread();
        void StopProcessThread();

    protected:
        VisionCom::VisionController m_ctrl;
        
        // boost::mutex -> std::mutex 교체
        std::mutex m_mutex;

    private:
        struct Impl;
        // 이미 스마트 포인터(std::unique_ptr)가 사용되고 있음
        std::unique_ptr<Impl> m_impl;

        void SendResultToSink(int requestId, const std::vector<std::string>& results);
        void PacketLoop();
    };
} // namespace DVH_VAT
