#include "VMFramework/Load1/VatAdapterLoad1Ex.h"
#include "VMFramework/Load1/Strategies/CPickPlaceSequenceStrategy.h"
#include "Orchestrator.h"

Load1Parts parts;
VMF_Load1::VatAdapterLoad1Ex adapter(&parts);
VMF::Orchestrator orchestrator;

bool ok = orchestrator.StartSequence<VMF_Load1::Strategies::CPickPlaceSequenceStrategy>(&adapter);
if (!ok)
{
    // 실행 실패 처리
}

orchestrator.AddObserver(
    [](const VMF::VisionResultPayload& payload)
    {
        for (size_t i = 0; i < payload.results.size(); ++i)
        {
            OutputDebugStringA((payload.results[i] + "\n").c_str());
        }
    });
