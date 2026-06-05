#pragma once

// Canonical public header for VAT sequence orchestration.
// NOTE: `bin/include` is treated as SDK export surface.
#include "CVatEngineObserverAdapter.h"

namespace VMF
{
 // Backward-compatibility: keep existing SDK names.
 using CVatEngineObserverAdapter = CVatSequenceOrchestrator;
 using CVatEngineUiAdapter = CVatSequenceOrchestrator;
 using CVatEngineFacade = CVatSequenceOrchestrator;
}
