#include "stdafx.h"
#include "SequenceStrategyBase.h"

namespace DVH_VAT
{
    SequenceStrategyBase::SequenceStrategyBase()
        :  m_adapter(nullptr)
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

