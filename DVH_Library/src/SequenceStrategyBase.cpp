#include "stdafx.h"
#include "SequenceStrategyBase.h"

namespace DVH_VAT
{
    SequenceStrategyBase::SequenceStrategyBase()
        :  m_adapter(nullptr)
    {
    }

    // 소멸자 (가상 함수 테이블 Export를 위해 구현부 필수)
    SequenceStrategyBase::~SequenceStrategyBase()
    {
    }

    void SequenceStrategyBase::SetActuator(IVatActuator* adapter)
    {
        m_adapter = adapter;
    }

    IVatActuator* SequenceStrategyBase::GetActuator()
    {
        return m_adapter;
    }
}

