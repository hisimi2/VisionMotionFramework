#pragma once

#include "Types.h"

namespace DVH_VAT
{
    class IVatSequenceStrategy
    {

    public:
        virtual ~IVatSequenceStrategy() {}

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

        // 6. IVatSequenceStrategy 인터페이스 구현               
        virtual void SetActuator(IVatActuator* adapter) = 0;

        // 7. IVatSequenceStrategy 인터페이스 구현
        virtual IVatActuator* GetActuator() = 0;

    };

    typedef boost::shared_ptr<IVatSequenceStrategy> VatSequenceStrategyPtr;
}
