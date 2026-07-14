#pragma once

#include "ITask.h"
#include "Context.h"
#include "IActuator.h"

#include <mutex>
#include <chrono>
#include <string>
#include <exception>

namespace VMF
{
/// <summary>
    /// 비동기(비차단) 작업 스텝의 공통 기초 클래스입니다.
    /// 각 스텝은 상태를 가지며 OnInitialize / OnPoll 패턴으로 실행됩니다.
    /// 스레드 안전을 위해 내부적으로 std::mutex를 사용하며, 타임아웃(데드라인) 기능을 제공합니다.
    /// 
    /// [Task-specific VisionParams]
    /// Task별로 독립적인 VisionParams를 설정할 수 있습니다.
    /// SetTaskParams()로 설정하면, GetTaskSeqParamAs / GetTaskVisionParamAs 로 조회합니다.
    /// 
    /// 사용 예:
    ///   auto moveTask = std::make_shared<TaskLoad1MoveToStartPosition>();
    ///   VisionParams moveParams;
    ///   moveParams.visionParams["TargetX"] = "125.3";
    ///   moveTask->SetTaskParams(moveParams);
    ///   seq->AddTask(moveTask);
    /// </summary>
    class NonBlockingTaskBase : public ITask
    {
    public:

        static const int CS_INITIALIZING    = -1; // 초기화 상태 (공통)
        static const int CS_ERROR           = -2; // 에러 상태 (공통)
        static const int CS_IDLE            = -3; // 대기 상태 (공통)

        /// <summary>
        /// 기본 생성자. 내부 상태와 플래그를 초기화합니다.
        /// </summary>
        NonBlockingTaskBase()
            : m_state_(CS_INITIALIZING)
            , m_initialized_(false)
            , m_hasDeadline_(false)
        {
        }

        /// <summary>
        /// 가상 소멸자. 파생 클래스에서 안전하게 파괴될 수 있도록 합니다.
        /// </summary>
        ~NonBlockingTaskBase() override = default;

        /// <summary>
        /// 스텝 실행 진입점입니다.
        /// - Stop 요청이 있으면 에러로 종료합니다.
        /// - 최초 호출 시 OnInitialize를 호출합니다.
        /// - 이후에는 OnPoll을 호출하여 상태를 진행합니다.
        /// 예외는 캐치되어 컨텍스트에 마지막 오류로 기록되고 TR_ERROR를 반환합니다.
        /// </summary>
        /// <param name="ctx">공유 실행 컨텍스트</param>
        /// <param name="actuator">Actuator 인터페이스 (존재하지 않을 수 있음)</param>
        /// <returns>TaskResult 값</returns>
        TaskResult Execute(Context& ctx, IActuator* actuator) override
        {
            std::lock_guard<std::mutex> lg(m_mutex_); 

            if (ctx.GetStopRequested())
            {
                ctx.SetLastError("NonBlockingTaskBase: Stop requested");
                m_state_ = CS_ERROR;
                return TR_ERROR;
            }

            if (!m_initialized_)
            {
                m_initialized_ = true;
                try
                {
                    OnInitialize(ctx);
                }
                catch (const std::exception& ex)
                {
                    ctx.SetLastError(std::string("NonBlockingTaskBase: exception in OnInitialize: ") + ex.what());
                    m_state_ = CS_ERROR;
                    return TR_ERROR;
                }
                catch (...)
                {
                    ctx.SetLastError("NonBlockingTaskBase: unknown exception in OnInitialize");
                    m_state_ = CS_ERROR;
                    return TR_ERROR;
                }
            }

            try
            {
                return OnPoll(ctx, actuator);
            }
            catch (const std::exception& ex)
            {
                ctx.SetLastError(std::string("NonBlockingTaskBase: exception in OnPoll: ") + ex.what());
                m_state_ = CS_ERROR;
                return TR_ERROR;
            }
            catch (...)
            {
                ctx.SetLastError("NonBlockingTaskBase: unknown exception in OnPoll");
                m_state_ = CS_ERROR;
                return TR_ERROR;
            }
        }

        /// <summary>
        /// 강제 중단을 요청합니다. 내부 상태를 에러 상태로 전환합니다.
        /// </summary>
        void Abort() override
        {
            std::lock_guard<std::mutex> lg(m_mutex_);
            m_state_ = CS_ERROR;
        }

        /// <summary>
        /// 지정한 상태로 진입합니다. 데드라인(타임아웃)은 초기화됩니다.
        /// </summary>
        /// <param name="newState">진입할 상태 식별자</param>
        void EnterState(int newState) override
        {
            m_state_ = newState;
            m_hasDeadline_ = false;
        }

        /// <summary>
        /// 스텝의 이름을 반환합니다. 파생 클래스에서 구현해야 합니다.
        /// </summary>
        std::string GetName() const override = 0;

        /// <summary>
        /// 외부에서 이 스텝의 뮤텍스를 획득할 수 있게 합니다.
        /// 스레드 동기화를 위해 필요할 때 사용합니다.
        /// </summary>
        /// <returns>내부 뮤텍스 참조</returns>
        std::mutex& GetMutex()
        {
            return m_mutex_;
        }

        /// <summary>
        /// 오류 메시지를 컨텍스트에 기록하고 지정한 상태로 전이한 후 TR_ERROR를 반환합니다.
        /// 파생 클래스에서 에러 처리 및 반환을 간단히 하기 위한 헬퍼입니다.
        /// </summary>
        TaskResult SetErrorAndReturn(Context& ctx, const std::string& msg) override
        {
            ctx.SetLastError(msg);
            EnterState(CS_ERROR);
            return TR_ERROR;
        }

        // =====================================================
        // [Task-specific VisionParams 지원]
        // =====================================================
        /// <summary>
        /// Task별 독립적인 파라미터를 설정합니다.
        /// Builder에서 Task 생성 후 호출하여 Task마다 다른 파라미터를 주입할 수 있습니다.
        /// </summary>
        /// <param name="params">Task 전용 VisionParams</param>
        void SetTaskParams(const VisionParams& params)
        {
            std::lock_guard<std::mutex> lg(m_mutex_);
            m_taskParams_ = params;
        }

        /// <summary>
        /// Task별 시퀀스 파라미터를 읽습니다.
        /// m_taskParams_.visionParams에서 키를 찾고, 없으면 defaultValue 반환
        /// </summary>
        template <typename T>
        T GetTaskSeqParamAs(Context& ctx, const std::string& key, const T& defaultValue = T()) const
        {
            (void)ctx;
            auto it = m_taskParams_.visionParams.find(key);
            if (it != m_taskParams_.visionParams.end())
            {
                T converted;
                if (detail::ParamConverter<T>::Convert(it->second, converted))
                    return converted;
            }
            return defaultValue;
        }

        /// <summary>
        /// Task별 비전 파라미터를 읽습니다.
        /// m_taskParams_.visionParams에서 키를 찾고, 없으면 defaultValue 반환
        /// </summary>
        template <typename T>
        T GetTaskVisionParamAs(Context& ctx, const std::string& key, const T& defaultValue) const
        {
            (void)ctx;
            auto it = m_taskParams_.visionParams.find(key);
            if (it != m_taskParams_.visionParams.end())
            {
                T converted;
                if (detail::ParamConverter<T>::Convert(it->second, converted))
                    return converted;
            }
            return defaultValue;
        }

        /// <summary>
        /// Task별 visionPositions의 첫 번째 위치를 제거하지 않고 조회합니다.
        /// </summary>
        bool PeekTaskVisionPosition(VisionPosition& outPos) const
        {
            if (m_taskParams_.visionPositions.empty())
                return false;
            outPos = m_taskParams_.visionPositions.back();
            return true;
        }

        /// <summary>
        /// Task별 visionPositions의 첫 번째 위치를 꺼내고 제거합니다.
        /// </summary>
        bool PopTaskVisionPosition(VisionPosition& outPos)
        {
            if (m_taskParams_.visionPositions.empty())
                return false;
            outPos = m_taskParams_.visionPositions.front();
            m_taskParams_.visionPositions.erase(m_taskParams_.visionPositions.begin());
            return true;
        }

        /// <summary>
        /// Task별 visionPositions가 비어있는지 확인합니다.
        /// </summary>
        bool IsTaskVisionPositionEmpty() const
        {
            return m_taskParams_.visionPositions.empty();
        }

    protected:
        /// <summary>
        /// 스텝 초기화 시 호출되는 콜백입니다. 파생 클래스에서 초기화 로직을 구현합니다.
        /// (Execute에서 최초 한 번 호출됨)
        /// </summary>
        virtual void OnInitialize(Context& ctx) = 0;

        /// <summary>
        /// 스텝 진행 시 반복적으로 호출되는 폴링 콜백입니다.
        /// 상태 전이 및 작업 완료 판정을 이곳에서 수행합니다.
        /// </summary>
        /// <returns>다음 동작을 나타내는 TaskResult</returns>
        virtual TaskResult OnPoll(Context& ctx, IActuator* actuator) = 0;

        /// <summary>
        /// 공통 상태(CBS_*)로 진입합니다. 데드라인은 초기화됩니다.
        /// </summary>
        /// <param name="state">진입할 공통 상태</param>
        void EnterCommonState(int state)
        {
            m_state_ = state;
            m_hasDeadline_ = false;
        }

        /// <summary>
        /// 지정한 상태로 진입하고 타임아웃(밀리초)을 설정합니다.
        /// timeoutMs <= 0 이면 데드라인을 사용하지 않습니다.
        /// </summary>
        /// <param name="newState">진입할 상태</param>
        /// <param name="timeoutMs">데드라인까지의 시간(밀리초)</param>
        void EnterStateWithTimeout(int newState, long timeoutMs)
        {
            m_state_ = newState;

            if (timeoutMs <= 0)
            {
                m_hasDeadline_ = false;
            }
            else
            {
                m_deadline_ = std::chrono::steady_clock::now() + std::chrono::milliseconds(timeoutMs);
                m_hasDeadline_ = true;
            }
        }

        /// <summary>
        /// 현재 상태 식별자를 반환합니다.
        /// </summary>
        /// <returns>현재 상태 값</returns>
        int GetState() const
        {
            return m_state_;
        }

        /// <summary>
        /// 설정된 데드라인이 만료되었는지 검사합니다.
        /// 데드라인이 설정되지 않은 경우 false를 반환합니다.
        /// </summary>
        /// <returns>만료되었으면 true, 아니면 false</returns>
        bool IsDeadlineExpired() const
        {
            if (!m_hasDeadline_)
            {
                return false;
            }

            return std::chrono::steady_clock::now() >= m_deadline_;
        }

    private:
        int                                   m_state_;
        bool                                  m_initialized_;
        bool                                  m_hasDeadline_;
        
        // 동기화 및 시간 관리 멤버를 C++ 표준 라이브러리로 대체
        mutable std::mutex                    m_mutex_;
        std::chrono::steady_clock::time_point m_deadline_;

    protected:

        // Task-specific VisionParams (Builder에서 직접 주입)
        VisionParams                          m_taskParams_;
    };
}
