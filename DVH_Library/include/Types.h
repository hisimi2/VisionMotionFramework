#pragma once

#include <string>
#include <map>
#include <vector>
#include <cstdint>

// [v100] C++11 미지원 기능 대체용 Boost 라이브러리
#include <boost/thread/mutex.hpp>
#include <boost/thread/lock_guard.hpp>
#include <boost/thread/locks.hpp>              
#include <boost/thread/condition_variable.hpp> 
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/chrono.hpp>
#include <boost/enable_shared_from_this.hpp>


namespace DVH_VAT 
{
    class IDataRepository;
    class IVisionEventHandler;
    class IVatSequence;
    class IVatActuator;
    class VatCorrectionEngine;
    class SequenceBuilderBase;
    class VAT_Context;
    class AsyncSequenceRunner;

    // 스마트 포인터 타입 정의
    typedef boost::shared_ptr<VatCorrectionEngine>  VatEnginePtr;
    typedef boost::shared_ptr<IDataRepository>      DataRepositoryPtr;
    typedef boost::shared_ptr<IVisionEventHandler>  VisionEventHandlerPtr;
    typedef boost::shared_ptr<SequenceBuilderBase>  SequenceBuilderPtr;
    typedef boost::shared_ptr<VAT_Context>          VatContextPtr;
    typedef boost::shared_ptr<AsyncSequenceRunner>  AsyncSequenceRunnerPtr;
    typedef boost::unique_ptr<IVatSequence>         VatSequencePtr;
    typedef IVatActuator*                           VatActuatorPtr;
    
    typedef std::map<std::string, std::string>      StringMap; 
    typedef boost::lock_guard<boost::mutex>         LockGuardType;
    typedef boost::unique_lock<boost::mutex>        UniqueLockType;
    typedef boost::condition_variable               ConditionVariableType;
} // namespace DVH_VAT
