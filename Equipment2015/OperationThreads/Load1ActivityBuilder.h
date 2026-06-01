#pragma once
#include "ActivityBuilderBase.h"
#include "Load1TaskPick.h"
#include "Load1TaskPlace.h"

namespace OperationThread
{
    using namespace EC;
    class CLoad1ActivityBuilder : public ActivityBuilderBase
    {
        std::shared_ptr<Load1Parts> m_parts;

    protected:
        ActivityPtr Build(std::string Name) override
        {
            m_parts = std::make_shared<Load1Parts>();

            ActivityPtr activity(new Activity(Name));
            activity->AddTask(std::make_shared<Load1TaskPick>(m_parts));
            activity->AddTask(std::make_shared<Load1TaskPlace>(m_parts));

            return ActivityPtr(activity.release());
            
        }
    };
}
