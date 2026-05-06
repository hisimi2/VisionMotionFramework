#pragma once
#include "DVH_VAT_API.h"
#include "VAT_Context.h"
#include "VatSequence.h"
#include "IDataRepository.h"
#include "IVatActuator.h"
#include <boost/move/unique_ptr.hpp>

namespace DVH_VAT
{
    /// <summary>
    /// 특정 작업(Strategy)에 필요한 시퀀스를 구성(Build)하는 역할 클래스입니다.
    /// <para>
    /// 시퀀스 빌더는 시퀀스 이름을 기반으로 시퀀스 객체를 생성하는 팩토리 역할을 수행합니다.
    /// </para>
    /// </summary>
    /// <remarks>
    /// 주요 기능:
    /// 1. 시퀀스 생성 (Sequence Creation)
    /// 2. 태스크(Task) 조립 (Task Assembly)
    /// 3. 전략별 시퀀스 정의 (Definition per Strategy)
    ///
    /// 사용법:
    /// 1. SequenceBuilderBase를 상속받는 구체적인 빌더 클래스를 생성합니다.
    /// 2. BuildSequence 메서드를 오버라이드하여 시퀀스 생성 및 태스크 추가 로직을 구현합니다.
    /// 3. 전략 클래스에서 CreateBuilder()를 통해 인스턴스를 반환합니다.
    /// </remarks>
    class DVH_VAT_API SequenceBuilderBase
    {
    public:
        SequenceBuilderBase();
        virtual ~SequenceBuilderBase();

        /// <summary>
        /// 주어진 이름으로 시퀀스를 생성합니다.
        /// </summary>
        /// <param name="sequenceName">생성할 시퀀스의 이름</param>
        /// <returns>생성된 IVatSequence 객체의 소유권</returns>
        VatSequencePtr CreateSequence(std::string sequenceName);

    protected:
        /// <summary>
        /// 실제 시퀀스 생성 및 태스크 조립을 수행하는 가상 함수입니다. 하위 클래스에서 구현해야 합니다.
        /// </summary>
        /// <param name="sequenceName">시퀀스 이름</param>
        /// <returns>조립된 IVatSequence 객체의 소유권</returns>
        virtual VatSequencePtr BuildSequence(std::string sequenceName) = 0;
    };
}
