#include "stdafx.h"
#include "CPickPlaceSequenceBuilder.h"
#include "Sequence.h"
#include "VMFramework/Load1/Tasks/CPickPlaceTaskEx.h"

namespace VMF_Load1
{
    namespace Sequence
    {
        VMF::SequencePtr CPickPlaceSequenceBuilder::BuildSequence(std::string sequenceName)
        {
            VMF::SequencePtr seq(new VMF::Sequence(sequenceName));
            seq->AddTask(VMF::TaskPtr(new VMF_Load1::Task::CPickPlaceTaskEx()));
            return seq;
        }
    }
}
