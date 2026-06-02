#include "StdAfx.h"
#include "ActivityBuilderBase.h"

namespace EC
{
    ActivityBuilderBase::ActivityBuilderBase() {}
    ActivityBuilderBase::~ActivityBuilderBase() {}

    std::unique_ptr<IActivity> ActivityBuilderBase::Create()
    {
        return Build();
    }
} 
