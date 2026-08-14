#pragma once
#include "Types.h"

namespace VMF
{
    /**
     * @brief Task가 파라미터를 조회하기 위한 인터페이스
     *
     * Task는 Context에 직접 의존하지 않고 이 인터페이스를 통해
     * 파라미터를 조회합니다. 이를 통해 단위 테스트 시 Mock 구현이 가능합니다.
     *
     * @note Task가 다양해질 것을 대비하여 제네릭 StringMap 기반 파라미터 접근을 제공합니다.
     *       기능별 파라미터 키는 플러그인에서 네임스페이스로 정의합니다.
     */
    class IParamProvider
    {
    public:
        virtual ~IParamProvider() = default;

        /// <summary>
        /// Task 실행 파라미터를 조회합니다.
        /// </summary>
        virtual TaskParams GetTaskParams() const = 0;

        /// <summary>
        /// 실행 파라미터 조회 (문자열)
        /// </summary>
        virtual std::string GetExecutionParam(const std::string& key) const = 0;

        /// <summary>
        /// 실행 파라미터 설정 (문자열)
        /// </summary>
        virtual void SetExecutionParam(const std::string& key, const std::string& value) = 0;

        /// <summary>
        /// 실행 파라미터 설정 (정수)
        /// </summary>
        virtual void SetExecutionParam(const std::string& key, int value) = 0;

        /// <summary>
        /// 실행 파라미터 설정 (실수)
        /// </summary>
        virtual void SetExecutionParam(const std::string& key, double value) = 0;

        /// <summary>
        /// VisionPositions 목록을 조회합니다.
        /// </summary>
        virtual std::vector<VisionPosition> GetVisionPositions() const = 0;

        /// <summary>
        /// 마지막 VisionPosition을 조회합니다.
        /// </summary>
        virtual bool PeekVisionPosition(VisionPosition& outPos) const = 0;

        /// <summary>
        /// 하위 호환성을 위한 문자열 파라미터 조회
        /// @deprecated 새로운 코드에서는 GetExecutionParam()을 사용하세요.
        /// </summary>
        virtual std::string GetParam(const std::string& key) const = 0;
    };
} // namespace VMF
