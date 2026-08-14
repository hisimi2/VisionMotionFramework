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

    using StringMap = std::map<std::string, std::string>;

    struct VisionPosition
    {
        std::vector<double> pos;
        int locateId;
        int visionRequestId;

        VisionPosition() : locateId(0), visionRequestId(0) {}

        // std::move 를 이용하여 파라미터 복사 오버헤드 축소
        VisionPosition(std::vector<double> _pos, int _locateId, int _visionRequestId)
            : pos(std::move(_pos)), locateId(_locateId), visionRequestId(_visionRequestId) {
        }
    };

    /**
     * @brief Context에서 Task별로 사용하는 파라미터를 저장하는 구조체
     *
     * Task가 다양해질 것을 대비하여 제네릭 StringMap 기반 파라미터 저장소를 사용합니다.
     * 기능별 파라미터 키는 플러그인에서 네임스페이스로 정의합니다.
     *
     * 사용 예:
     *   TaskParams params;
     *   params.SetExecutionParam("Setup.TIMEOUT_MOVE_MS", 7000);
     *   params.SetExecutionParam("ExecuteScan.SCAN_END_Y", 200.0);
     *   params.visionPositions.push_back(VisionPosition({0.0, 0.0, 0.0}, 0, 1));
     *
     * 조회:
     *   int timeout = params.GetExecutionParam<int>("Setup.TIMEOUT_MOVE_MS", 5000);
     *   double scanEndY = params.GetExecutionParam<double>("ExecuteScan.SCAN_END_Y", 100.0);
     */
    struct TaskParams
    {
        /// <summary>
        /// Task 실행 파라미터 (기능별 키 네임스페이스로 관리)
        /// 예: "Setup.TIMEOUT_MOVE_MS", "ExecuteScan.SCAN_END_Y", "Finish.TIMEOUT_MOVE_MS"
        /// </summary>
        StringMap executionParams;

        /// <summary>
        /// 이동 위치 파라미터 (측정/안전/홈 위치 등)
        /// </summary>
        std::vector<VisionPosition> visionPositions;

        /// <summary>
        /// 실행 파라미터 조회 (타입 안전)
        /// </summary>
        template<typename T>
        T GetExecutionParam(const std::string& key, T defaultValue = T()) const
        {
            auto it = executionParams.find(key);
            if (it != executionParams.end())
            {
                T converted;
                if (detail::ParamConverter<T>::Convert(it->second, converted))
                    return converted;
            }
            return defaultValue;
        }

        /// <summary>
        /// 실행 파라미터 설정 (문자열)
        /// </summary>
        void SetExecutionParam(const std::string& key, const std::string& value)
        {
            executionParams[key] = value;
        }

        /// <summary>
        /// 실행 파라미터 설정 (정수)
        /// </summary>
        void SetExecutionParam(const std::string& key, int value)
        {
            executionParams[key] = std::to_string(value);
        }

        /// <summary>
        /// 실행 파라미터 설정 (실수)
        /// </summary>
        void SetExecutionParam(const std::string& key, double value)
        {
            executionParams[key] = std::to_string(value);
        }
    };

// ── 파라미터 변환 헬퍼 (TaskParams 내부 템플릿에서 사용) ──
    namespace detail
    {
        template <typename T>
        struct ParamConverter
        {
            static bool Convert(const std::string& text, T& value)
            {
                std::istringstream iss(text);
                T temp;
                iss >> temp;

                if (iss.fail())
                    return false;

                value = temp;
                return true;
            }
        };

        template <>
        struct ParamConverter<std::string>
        {
            static bool Convert(const std::string& text, std::string& value)
            {
                value = text;
                return true;
            }
        };

        template <>
        struct ParamConverter<int>
        {
            static bool Convert(const std::string& text, int& value)
            {
                if (text.empty())
                    return false;

                char* endPtr = nullptr;
                const long parsed = std::strtol(text.c_str(), &endPtr, 10);

                if (endPtr == text.c_str() || *endPtr != '\0')
                    return false;

                value = static_cast<int>(parsed);
                return true;
            }
        };

        template <>
        struct ParamConverter<double>
        {
            static bool Convert(const std::string& text, double& value)
            {
                if (text.empty())
                    return false;

                char* endPtr = nullptr;
                const double parsed = std::strtod(text.c_str(), &endPtr);

                if (endPtr == text.c_str() || *endPtr != '\0')
                    return false;

                value = parsed;
                return true;
            }
        };

        template <>
        struct ParamConverter<bool>
        {
            static bool Convert(const std::string& text, bool& value)
            {
                if (text == "1" || text == "true" || text == "TRUE" || text == "True")
                {
                    value = true;
                    return true;
                }

                if (text == "0" || text == "false" || text == "FALSE" || text == "False")
                {
                    value = false;
                    return true;
                }

                return false;
            }
        };
    } // namespace detail

    using ComponentSetupPtr = std::shared_ptr<IComponentSetup>;
    using SequenceSetupPtr = std::shared_ptr<ISequenceSetup>;
    using VisionEnginePtr = std::shared_ptr<RunController>;
    using DataRepositoryPtr = std::shared_ptr<IDataRepository>;
    using VisionProcessorPtr = std::shared_ptr<IVisionClient>;
    using SequenceBuilderPtr = std::shared_ptr<SequenceBuilderBase>;
    using VisionContextPtr = std::shared_ptr<Context>;
    using AsyncExecutorPtr = std::shared_ptr<AsyncExecutor>;
    using SequencePtr = std::unique_ptr<ISequence>;
    using LockGuardType = std::lock_guard<std::mutex>;
    using UniqueLockType = std::unique_lock<std::mutex>;
    using ConditionVariableType = std::condition_variable;
    using VisionActuatorPtr = IActuator*;  // raw pointer (소유권 없음, Orchestrator/RunController가 생명주기 관리)
} // namespace VMF
