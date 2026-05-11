#include "stdafx.h"
#include "SequenceStrategyBase.h"

namespace VMF
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

