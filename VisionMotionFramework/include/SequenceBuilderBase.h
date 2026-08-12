#pragma once
#include "VMF_API.h"
#include "Types.h"
#include "Sequence.h"


namespace VMF
{
    class Context;

    /// <summary>
    /// 특정 작업(Strategy)에 필요한 시퀀스를 구성(Build)하는 역할 클래스입니다.
    /// </summary>
    /// <remarks>
    /// 주요 기능:
    /// 1. 시퀀스 생성 (Sequence Creation)
    /// 2. 태스크(Task) 조립 (Task Assembly)
    /// 3. 전략별 시퀀스 정의 (Definition per Strategy)
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
        /// </summary>
        /// <param name="sequenceName">생성할 시퀀스의 이름</param>
        /// <returns>생성된 ISequence 객체의 소유권</returns>
        SequencePtr CreateSequence(const std::string& sequenceName)
        {
            return BuildSequence(sequenceName);
        }

        /// <summary>
        /// 시퀀스 실행 전 Context에 필요한 파라미터를 설정합니다.
        /// SequenceBuilder가 시퀀스 실행 전에 필요한 파라미터를 Context에 저장합니다.
        /// </summary>
        /// <param name="ctx">파라미터를 설정할 Context</param>
        virtual void ConfigureContext(VMF::Context& ctx) {}

    protected:
        /// <summary>
        /// 실제 시퀀스 생성 및 태스크 조립을 수행하는 가상 함수입니다. 하위 클래스에서 구현해야 합니다.
        /// </summary>
        /// <param name="sequenceName">시퀀스 이름</param>
        /// <returns>조립된 ISequence 객체의 소유권</returns>
        virtual SequencePtr BuildSequence(const std::string& sequenceName) = 0;

        IActuator* m_actuator = nullptr; // 시퀀스 빌더에서 사용할 액추에이터 인터페이스
    };
}
