#include "pch.h"
#include "PLVILoad1Strategy.h"

namespace VMF_Sample
{
    PLVILoad1Strategy::PLVILoad1Strategy()
    {
    }

    PLVILoad1Strategy::~PLVILoad1Strategy()
    {
    }

    std::string PLVILoad1Strategy::GetSequenceName() const
    {
        return "PLVILoad1";  // Direct Mode용 시퀀스 이름
    }

    SequenceBuilderPtr PLVILoad1Strategy::CreateBuilder()
    {
        // Direct Mode에서는 Builder가 실제로 사용되지 않으므로 nullptr 반환
        return nullptr;
    }
}
