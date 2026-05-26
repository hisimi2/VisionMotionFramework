#include "stdafx.h"
#include "Sequence.h"
#include "CPickPlaceSequenceBuilder.h"
#include "AutoThread/CPickPlaceTaskEx.h"

namespace AutoThread
{
    namespace Sequence
    {
        VMF::SequencePtr CPickPlaceSequenceBuilder::BuildSequence(std::string sequenceName)
        {
            VMF::SequencePtr seq(new VMF::Sequence(sequenceName));
            seq->AddTask(VMF::TaskPtr(new Task::CPickPlaceTaskEx()));
            return seq;
        }
    }
}
