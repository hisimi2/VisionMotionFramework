#pragma once

#include <string>
#include <map>
#include <vector>
#include <cstdint>
#include <memory>              // std::shared_ptr, std::unique_ptr, std::make_shared 등
#include <mutex>               // std::mutex, std::lock_guard, std::unique_lock
#include <condition_variable>  // std::condition_variable
#include <chrono>              // std::chrono

namespace VMF 
{
    class IDataRepository;
    class IVisionEventHandler;
    class IVatSequence;
    class IVatActuator;
    class VatCorrectionEngine;
    class SequenceBuilderBase;
    class VAT_Context;
    class AsyncSequenceRunner;
    
    using VatEnginePtr           = std::shared_ptr<VatCorrectionEngine>;
    using DataRepositoryPtr      = std::shared_ptr<IDataRepository>;
    using VisionEventHandlerPtr  = std::shared_ptr<IVisionEventHandler>;
    using SequenceBuilderPtr     = std::shared_ptr<SequenceBuilderBase>;
    using VatContextPtr          = std::shared_ptr<VAT_Context>;
    using AsyncSequenceRunnerPtr = std::shared_ptr<AsyncSequenceRunner>;
    
    // boost::unique_ptr가 아니라 기존에도 std::unique_ptr 였으나 C++11 표준이므로 유지
    using VatSequencePtr         = std::unique_ptr<IVatSequence>;
    
    using VatActuatorPtr         = IVatActuator*;
    using StringMap              = std::map<std::string, std::string>; 
    
    // boost 스레드 동기화 객체를 C++11 표준 라이브러리로 대체
    using LockGuardType          = std::lock_guard<std::mutex>;
    using UniqueLockType         = std::unique_lock<std::mutex>;
    using ConditionVariableType  = std::condition_variable;
} // namespace VMF
