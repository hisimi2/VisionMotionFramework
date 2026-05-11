#pragma once
#include "VMF_API.h"
#include "IVatSequenceStrategy.h"
#include "VatSequence.h"
#include <string>

namespace VMF
{
    /// <summary>
    /// 시퀀스 실행 전략(Strategy)을 정의하는 기본 추상 클래스입니다.
    /// <para>
    /// 각 구체적인 전략 클래스는 이 클래스를 상속받아 시퀀스 이름, 빌더, 파라미터 설정 등을 구현해야 합니다.
    /// </para>
    /// </summary>
    class VMF_API SequenceStrategyBase : public IVatSequenceStrategy
    {
    protected:
        IVatActuator* m_adapter;

    public:
        /// <summary>
        /// SequenceStrategyBase의 생성자입니다.
        /// </summary>
        SequenceStrategyBase();

        /// <summary>
        /// SequenceStrategyBase의 가상 소멸자입니다.
        /// </summary>
        ~SequenceStrategyBase() override = default;

        /// <summary>
        /// 실행할 시퀀스의 이름을 반환합니다.
        /// </summary>
        /// <returns>시퀀스 이름 문자열</returns>
        std::string GetSequenceName() const override = 0;

        /// <summary>
        /// 이 전략에서 사용할 SequenceBuilder 객체를 생성하여 반환합니다.
        /// </summary>
        /// <returns>생성된 SequenceBuilder의 스마트 포인터</returns>
        SequenceBuilderPtr CreateBuilder() override = 0;

        /// <summary>
        /// 시퀀스 실행에 필요한 파라미터를 Context에 설정(주입)합니다.
        /// </summary>
        /// <param name="context">설정할 VatContext 객체</param>
        void ConfigureParams(VatContextPtr context) override = 0;

        /// <summary>
        /// 시퀀스에서 사용할 데이터 저장소(Repository)를 생성하여 반환합니다.
        /// </summary>
        /// <returns>생성된 DataRepository의 스마트 포인터</returns>
        DataRepositoryPtr CreateRepository() override = 0;

        /// <summary>
        /// 시퀀스에서 사용할 비전 이벤트 처리기(Vision Processor)를 생성하여 반환합니다.
        /// </summary>
        /// <returns>생성된 VisionEventHandler의 스마트 포인터</returns>
        VisionEventHandlerPtr CreateVisionProcessor() override = 0;

        /// <summary>
        /// 액추에이터(하드웨어 제어 인터페이스)를 설정합니다.
        /// IVatSequenceStrategy 인터페이스 구현입니다.
        /// </summary>
        /// <param name="adapter">설정할 액추에이터 포인터</param>
        void SetActuator(IVatActuator* adapter) override;

        /// <summary>
        /// 현재 설정된 액추에이터를 반환합니다.
        /// IVatSequenceStrategy 인터페이스 구현입니다.
        /// </summary>
        /// <returns>액추에이터 포인터</returns>
        IVatActuator* GetActuator() override;
    };
}
