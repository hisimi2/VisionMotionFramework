#include "StdAfx.h"
#include "ActivityBuilderBase.h"
#include "IActivity.h"

namespace EC
{
    std::unique_ptr<IActivity> ActivityBuilderBase::Create()
    {
        return Build();
    }
}
