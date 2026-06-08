#include "stdafx.h"
#include "SequenceSetupBase.h"

namespace VMF
{
    SequenceSetupBase::SequenceSetupBase()
        :  m_adapter(nullptr)
    {
    }

    void SequenceSetupBase::SetActuator(IActuator* adapter)
    {
        m_adapter = adapter;
    }

    IActuator* SequenceSetupBase::GetActuator()
    {
        return m_adapter;
    }
}

