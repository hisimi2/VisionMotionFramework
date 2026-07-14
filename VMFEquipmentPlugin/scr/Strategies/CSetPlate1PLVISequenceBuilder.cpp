#include "pch.h"
#include "CSetPlate1PLVISequenceBuilder.h"

#include "scr\Tasks\CSetPlate1PLVIMoveToScanStartTask.h"
#include "scr\Tasks\CSetPlate1PLVIPerformScanTask.h"

namespace VMF_PLUGIN
{

    VMF::SequencePtr CSetPlate1PLVISequenceBuilder::BuildSequence(
        const std::string& sequenceName) 
    {
        VMF::SequencePtr seq(new VMF::Sequence(sequenceName));

        seq->AddTask(VMF::TaskPtr(new CSetPlate1PLVIMoveToScanStartTask()));
        seq->AddTask(VMF::TaskPtr(new CSetPlate1PLVIPerformScanTask()));

        return VMF::SequencePtr(seq.release());
    }



}
