#pragma once

#include "DVH_VAT_API.h"
#include "VisionProcessorBase.h"
#include "VisionMemoryKeys.h"

#include <vector>
#include <string>

namespace DVH_VAT
{
    class DVH_VAT_API VisionMemoryProcessor : public VisionProcessorBase
    {
    public:
        VisionMemoryProcessor();
        
        // C++11/14: 다형성 클래스의 소멸자에 명시적으로 override 지정
        ~VisionMemoryProcessor() override;

        // C++11/14: 부모(VisionProcessorBase)로부터 상속받은 가상 함수들에 virtual 생략 및 override 명시
        bool RequestSetCokAsync(const StringMap& params) override;
        bool RequestInspReadyAsync(const StringMap& params) override;
        bool RequestMeasureAsync(const StringMap& params) override;
        bool RequestDeviceCheckAsync(const StringMap& params) override;
        bool RequestLightAsync(const StringMap& params) override;

        void OnSetCok(const ByteArray& body) override;
        void OnInspReady(const ByteArray& body) override;
        void OnMeasure(const ByteArray& body) override;
        void OnDeviceCheck(const ByteArray& body) override;
        void OnLight(const ByteArray& body) override;

        void Process() override;

    private:
        // C++11/14: 복사 생성자 및 대입 연산자 호출 방지를 구현 없이 = delete 로 명시적 처리
        VisionMemoryProcessor(const VisionMemoryProcessor&) = delete;
        VisionMemoryProcessor& operator=(const VisionMemoryProcessor&) = delete;

        std::vector<std::string> ParseMeasureBody(const ByteArray& body);
    };
}
