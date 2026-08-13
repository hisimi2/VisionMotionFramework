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

     /**
     * @brief SetPlate1 PLVI Setup Task 전용 파라미터
     */
    struct SetPlate1PLVISetupParams
    {
        int         timeoutMoveMs = 7000;
        double      triggerIntervalMm = 1.8;
        VisionPosition startPos;
    };

    /**
     * @brief SetPlate1 PLVI ExecuteScan Task 전용 파라미터
     */
    struct SetPlate1PLVIExecuteScanParams
    {
        int         timeoutMoveMs = 7000;
        int         timeoutResultMs = 10000;
        double      scanEndY = 200.0;
    };

    /**
     * @brief SetPlate1 PLVI Finish Task 전용 파라미터
     */
    struct SetPlate1PLVIFinishParams
    {
        int         timeoutMoveMs = 7000;
    };

    /**
     * @brief Context에서 Task별로 사용하는 파라미터를 저장하는 구조체
     * 
     * Task가 다양해질 것을 대비하여 Task별 파라미터 구조체를 분리합니다.
     * 각 Task는 자신의 파라미터 구조체를 통해 Context에서 파라미터를 조회합니다.
     * 하위 호환성을 위해 visionParams(StringMap)도 함께 유지합니다.
     */
    struct TaskParams
    {
        SetPlate1PLVISetupParams        setup;
        SetPlate1PLVIExecuteScanParams  executeScan;
        SetPlate1PLVIFinishParams       finish;
        std::vector<VisionPosition>     visionPositions;
        StringMap                       visionParams;  // 하위 호환성용 문자열 파라미터 맵
    };

    /**
     * @brief 하위 호환성을 위한 기존 VisionParams 구조체
     * @deprecated 새로운 코드에서는 TaskParams를 사용하세요.
     */
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
