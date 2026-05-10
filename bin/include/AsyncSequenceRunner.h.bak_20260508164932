#pragma once

#include "DVH_VAT_API.h"
#include "Types.h"
#include <memory> // boost 스마트 포인터 대신 C++ 표준 라이브러리 사용
#include <vector>
#include <string>

namespace DVH_VAT 
{
    class VAT_Context;
    class IVatSequence;
    class IVatActuator;
    class IResultSink;

    class DVH_VAT_API AsyncSequenceRunner 
    {
    public:
        AsyncSequenceRunner();
        
        // 가상 함수가 존재하므로 안전한 상속을 위해 가상 소멸자로 변경하는 것을 권장합니다.
        virtual ~AsyncSequenceRunner();

        // boost::unique_ptr, boost::shared_ptr -> std::unique_ptr, std::shared_ptr 교체
        bool Start(std::unique_ptr<IVatSequence> seq, std::shared_ptr<VAT_Context> ctx, IVatActuator* actuator);
        
        void Abort();
        void Stop();
        bool IsRunning() const;
        bool WaitForCompletion(int timeoutMs = -1);

        virtual void SetResultSink(IResultSink* sink);

        void SendResult(int requestId, const std::string& status);

    private:
        struct Impl;
        
        // boost::scoped_ptr은 소유권 이전이 불가능한 포인터였으나, 모던 C++에서는 std::unique_ptr로 통합하여 대체합니다.
        std::unique_ptr<Impl> m_impl;

        void SendResultToSink(int requestId, const std::vector<std::string>& results);
    };
}
