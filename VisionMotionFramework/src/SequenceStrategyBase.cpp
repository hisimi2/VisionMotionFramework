#include "stdafx.h"
#include "SequenceStrategyBase.h"

namespace VMF
{
    SequenceStrategyBase::SequenceStrategyBase()
        :  m_adapter(nullptr)
    {
    }

    void SequenceStrategyBase::SetActuator(IActuator* adapter)
    {
        m_adapter = adapter;
    }

    IActuator* SequenceStrategyBase::GetActuator()
    {
        return m_adapter;
    }
}

