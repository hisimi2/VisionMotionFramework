#pragma once

#include "Types.h"
#include <string>
#include <memory>

namespace VMF
{
    class IActuator; // GetActuator/SetActuator 등을 위한 전방 선언 추가

    class ISequenceStrategy
    {
    public:
        virtual ~ISequenceStrategy() = default;

        // 1. 실행할 시퀀스 이름 반환
        virtual std::string GetSequenceName() const = 0;

        // 2. 사용할 SequenceBuilder 생성
        virtual SequenceBuilderPtr CreateBuilder() = 0;

        // 3. 실행 파라미터 설정 (Context에 주입)
        virtual void ConfigureParams(VatContextPtr context) = 0;

        // 4. 사용할 데이터 연결소스 반환 
        virtual DataRepositoryPtr CreateRepository() = 0;

        // 5. 사용할 시퀀스 생성기 반환 
        virtual VisionEventHandlerPtr CreateVisionProcessor() = 0;

        // 6. 엑츄에이터(어댑터) 설정   
        virtual void SetActuator(IActuator* adapter) = 0;

        // 7. 설정된 엑츄에이터 반환
        virtual IActuator* GetActuator() = 0;
    };

    using SequenceStrategyPtr = std::shared_ptr<ISequenceStrategy>;
}
