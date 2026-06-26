#pragma once

#include <string> 
#include <map> 
#include <vector> 
#include <cstdint> 
#include <memory>              
#include <mutex>               
#include <condition_variable>  
#include <chrono>              

namespace VMF 
{
    class IComponentSetup;
    class ISequenceSetup;
    class IDataRepository;
    class IVisionProcessor;
    class ISequence;
    class IActuator;
    class RunController;
    class SequenceBuilderBase;
    class Context;
    class AsyncExecutor;
    
    using ComponentSetupPtr     = std::shared_ptr<IComponentSetup>;
    using SequenceSetupPtr      = std::shared_ptr<ISequenceSetup>;
    using VisionEnginePtr       = std::shared_ptr<RunController>;
    using DataRepositoryPtr     = std::shared_ptr<IDataRepository>;
    using VisionProcessorPtr    = std::shared_ptr<IVisionProcessor>;
    using SequenceBuilderPtr    = std::shared_ptr<SequenceBuilderBase>;
    using VisionContextPtr      = std::shared_ptr<Context>;
    using AsyncExecutorPtr      = std::shared_ptr<AsyncExecutor>;
    using SequencePtr           = std::unique_ptr<ISequence>;
    using StringMap             = std::map<std::string, std::string>; 
    using LockGuardType         = std::lock_guard<std::mutex>;
    using UniqueLockType        = std::unique_lock<std::mutex>;
    using ConditionVariableType = std::condition_variable;
    using VisionActuatorPtr     = IActuator*;  // raw pointer (소유권 없음, Orchestrator/RunController가 생명주기 관리)
} // namespace VMF
