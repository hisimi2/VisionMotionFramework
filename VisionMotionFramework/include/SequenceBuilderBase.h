#pragma once
#include "VMF_API.h"
#include "Context.h"
#include "Sequence.h"
#include "IDataRepository.h"
#include "IActuator.h"

namespace VMF
{
    /// <summary>
    /// 특정 작업(Strategy)에 필요한 시퀀스를 구성(Build)하는 역할 클래스입니다.
    /// <para>
    /// 시퀀스 빌더는 시퀀스 이름을 기반으로 시퀀스 객체를 생성하는 팩토리 역할을 수행합니다.
    /// </para>
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
        SequenceBuilderBase();
        virtual ~SequenceBuilderBase();

        /// <summary>
        /// 주어진 이름으로 시퀀스를 생성합니다.
        /// </summary>
        /// <param name="sequenceName">생성할 시퀀스의 이름</param>
        /// <returns>생성된 ISequence 객체의 소유권</returns>
        SequencePtr CreateSequence(const std::string& sequenceName);

    protected:
        /// <summary>
        /// 실제 시퀀스 생성 및 태스크 조립을 수행하는 가상 함수입니다. 하위 클래스에서 구현해야 합니다.
        /// </summary>
        /// <param name="sequenceName">시퀀스 이름</param>
        /// <returns>조립된 ISequence 객체의 소유권</returns>
        virtual SequencePtr BuildSequence(const std::string& sequenceName) = 0;

        // ============================================================================
        // VisionParams 헬퍼 메서드
        // Builder 파생 클래스에서 VisionParams 조립 시 사용
        // ============================================================================

        /// <summary>
        /// VisionParams에 문자열 파라미터를 설정합니다.
        /// </summary>
        void SetParam(VisionParams& params, const std::string& key, const std::string& value);

        /// <summary>
        /// VisionParams에 숫자(double) 파라미터를 문자열로 변환하여 설정합니다.
        /// </summary>
        void SetParam(VisionParams& params, const std::string& key, double value);

        /// <summary>
        /// VisionParams에 3축 비전 검사 위치를 추가합니다.
        /// </summary>
        void AddVisionPoint(VisionParams& params, int locateId, int requestId, double x, double y, double z);

        /// <summary>
        /// VisionParams에 5축 비전 검사 위치를 추가합니다.
        /// </summary>
        void AddVisionPoint(VisionParams& params, int locateId, int requestId, double x, double y, double z, double t1, double t2);
    };
}
