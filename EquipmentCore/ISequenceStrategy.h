#pragma once

#include <string>
#include <memory>

namespace EC
{
    class SequenceBuilderBase;
    class Context;

    using SequenceBuilderPtr = std::shared_ptr<SequenceBuilderBase>;
    using ContextPtr = std::shared_ptr<Context>;

    class ISequenceStrategy
    {
    public:
        virtual ~ISequenceStrategy() = default;

        // 1. 실행할 시퀀스 이름 반환
        virtual std::string GetSequenceName() const = 0;

        // 2. 사용할 SequenceBuilder 생성
        virtual SequenceBuilderPtr CreateBuilder() = 0;

        // 3. 실행 파라미터 설정 (Context에 주입)
        virtual void ConfigureParams(ContextPtr context) = 0;
      
    };

    using SequenceStrategyPtr = std::shared_ptr<ISequenceStrategy>;
}
