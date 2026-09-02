#pragma once
#include "VMF_API.h"
#include "Types.h"
#include "Sequence.h"
#include "IActuator.h"

namespace VMF
{
    class Context;

    /// <summary>
    /// 특정 작업(Strategy)에 필요한 시퀀스를 구성(Build)하는 역할 클래스입니다.
    /// </summary>
    /// <remarks>
    /// 주요 책임:
    /// 1. 시퀀스 생성 (Sequence Creation)
    /// 2. 태스크(Task) 조립 (Task Assembly)
    /// 
    /// 중요:
    /// - 빌더는 Task를 조립만 하며, 파라미터 설정 로직은 포함하지 않습니다.
    /// - 파라미터 설정은 Strategy의 ConfigureContext()에서 직접 Context에 수행합니다.
    /// - 빌더의 ConfigureContext()는 선택적 훅(optional hook)이며, 기본 구현은 비어있습니다.
    ///   Strategy가 직접 Context를 설정하는 경우 빌더의 ConfigureContext()를 오버라이드할 필요가 없습니다.
    /// 
    /// 사용법:
    /// 1. SequenceBuilderBase를 상속받는 구체적인 빌더 클래스를 생성합니다.
    /// 2. BuildSequence 메서드를 오버라이드하여 시퀀스 생성 및 태스크 추가 로직을 구현합니다.
    /// 3. 전략 클래스에서 CreateBuilder()를 통해 인스턴스를 반환합니다.
    /// </remarks>
    class VMF_API SequenceBuilderBase
    {
    public:
        SequenceBuilderBase() = default;
        virtual ~SequenceBuilderBase() = default;

        /// <summary>
        /// 주어진 이름으로 시퀀스를 생성합니다.
        /// BuildSequence()를 호출하는 편의 메서드입니다.
        /// </summary>
        /// <param name="sequenceName">생성할 시퀀스의 이름</param>
        /// <returns>생성된 ISequence 객체의 소유권 (std::unique_ptr)</returns>
        SequencePtr CreateSequence(const std::string& sequenceName)
        {
            return BuildSequence(sequenceName);
        }

        /// <summary>
        /// 시퀀스 실행 전 Context에 필요한 파라미터를 설정하는 선택적 훅입니다.
        /// </summary>
        /// <details>
        /// 기본 구현은 아무것도 하지 않습니다.
        /// 
        /// 참고:
        /// - 대부분의 Strategy에서는 Strategy::ConfigureContext()에서 직접 Context에 파라미터를 설정합니다.
        /// - 빌더에서 파라미터 설정이 필요한 특수한 경우에만 이 메서드를 오버라이드하세요.
        /// - PLVI 예시: SetPlate1PLVISequenceBuilder는 이 메서드를 오버라이드하지 않으며,
        ///   SetPlate1PLVIStrategy::ConfigureContext()에서 직접 파라미터를 설정합니다.
        /// </details>
        /// <param name="ctx">파라미터를 설정할 Context</param>
        virtual void ConfigureContext(Context& ctx) { (void)ctx; }

        /// <summary>
        /// 빌더에서 사용할 액추에이터 인터페이스를 설정합니다.
        /// </summary>
        /// <details>
        /// Task 생성 시 액추에이터 정보가 필요한 경우 사용합니다.
        /// 기본값은 nullptr이며, 필요 시 Strategy나 외부에서 설정합니다.
        /// </details>
        void SetActuator(IActuator* actuator) { m_actuator = actuator; }

        /// <summary>
        /// 현재 설정된 액추에이터 인터페이스를 반환합니다.
        /// </summary>
        IActuator* GetActuator() const { return m_actuator; }

    protected:
        /// <summary>
        /// 실제 시퀀스 생성 및 태스크 조립을 수행하는 순수 가상 함수입니다.
        /// 파생 클래스에서 반드시 구현해야 합니다.
        /// </summary>
        /// <param name="sequenceName">시퀀스 이름</param>
        /// <returns>조립된 ISequence 객체의 소유권 (std::unique_ptr)</returns>
        virtual SequencePtr BuildSequence(const std::string& sequenceName) = 0;

    private:
        IActuator* m_actuator = nullptr; // 시퀀스 빌더에서 사용할 액추에이터 인터페이스 (선택 사항)
    };
}
