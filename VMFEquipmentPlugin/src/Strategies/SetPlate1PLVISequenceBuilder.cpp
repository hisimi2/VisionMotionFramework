#include "pch.h"
#include "SetPlate1PLVISequenceBuilder.h"
#include "SetPlate1PLVIStrategy.h"
#include "VisionMotionFramework\Context.h"

#include "src\Tasks\SetPlate1PLVISetup.h"
#include "src\Tasks\SetPlate1PLVIExecuteScan.h"
#include "src\Tasks\SetPlate1PLVIFinish.h"

using namespace VMF;
using namespace VMF_PLUGIN;

VMF::SequencePtr SetPlate1PLVISequenceBuilder::BuildSequence(const std::string& sequenceName)
{
    VMF::SequencePtr seq(new VMF::Sequence(sequenceName));

    seq->AddTask(std::make_shared<SetPlate1PLVISetup>());
    seq->AddTask(std::make_shared<SetPlate1PLVIExecuteScan>());
    seq->AddTask(std::make_shared<SetPlate1PLVIFinish>());

    return seq;
}
