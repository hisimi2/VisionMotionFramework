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
    class IDataRepository;
    class IVisionEventHandler;
    class ISequence;
    class IActuator;
    class RunController;
    class SequenceBuilderBase;
    class Context;
    class AsyncExecutor;
    
    using VatEnginePtr          = std::shared_ptr<RunController>;
    using DataRepositoryPtr     = std::shared_ptr<IDataRepository>;
    using VisionEventHandlerPtr = std::shared_ptr<IVisionEventHandler>;
    using SequenceBuilderPtr    = std::shared_ptr<SequenceBuilderBase>;
    using VatContextPtr         = std::shared_ptr<Context>;
    using AsyncExecutorPtr      = std::shared_ptr<AsyncExecutor>;
    using SequencePtr           = std::unique_ptr<ISequence>;
    using StringMap             = std::map<std::string, std::string>; 
    using LockGuardType         = std::lock_guard<std::mutex>;
    using UniqueLockType        = std::unique_lock<std::mutex>;
    using ConditionVariableType = std::condition_variable;
    using VatActuatorPtr        = IActuator*;
} // namespace VMF
