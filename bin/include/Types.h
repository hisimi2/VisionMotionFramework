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
    class IVisionClient;
    class ISequence;
    class IActuator;
    class RunController;
    class SequenceBuilderBase;
    class Context;
    class AsyncExecutor;

    using StringMap             = std::map<std::string, std::string>; 

    struct VisionPosition
    {
        std::vector<double> pos;
        int locateId;
        int visionRequestId;

        VisionPosition() : locateId(0), visionRequestId(0) {}

        // std::move 를 이용하여 파라미터 복사 오버헤드 축소
        VisionPosition(std::vector<double> _pos, int _locateId, int _visionRequestId)
            : pos(std::move(_pos)), locateId(_locateId), visionRequestId(_visionRequestId) {}
    };

    struct VisionParams
    {
        StringMap                       visionParams;
        std::vector<VisionPosition>     visionPositions;
    };

    using ComponentSetupPtr     = std::shared_ptr<IComponentSetup>;
    using SequenceSetupPtr      = std::shared_ptr<ISequenceSetup>;
    using VisionEnginePtr       = std::shared_ptr<RunController>;
    using DataRepositoryPtr     = std::shared_ptr<IDataRepository>;
    using VisionProcessorPtr    = std::shared_ptr<IVisionClient>;
    using SequenceBuilderPtr    = std::shared_ptr<SequenceBuilderBase>;
    using VisionContextPtr      = std::shared_ptr<Context>;
    using AsyncExecutorPtr      = std::shared_ptr<AsyncExecutor>;
    using SequencePtr           = std::unique_ptr<ISequence>;
    using LockGuardType         = std::lock_guard<std::mutex>;
    using UniqueLockType        = std::unique_lock<std::mutex>;
    using ConditionVariableType = std::condition_variable;
    using VisionActuatorPtr        = IActuator*;  // raw pointer (소유권 없음, Orchestrator/RunController가 생명주기 관리)
} // namespace VMF
