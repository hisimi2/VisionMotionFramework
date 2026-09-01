#pragma once
#include "Types.h"
#include <string>

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
    class VMF_API IParamProvider
    {
    public:
        virtual ~IParamProvider() = default;

        /// <summary>
        /// 지정된 Task 이름의 실행 파라미터를 조회합니다.
        /// </summary>
        /// <details>
        /// Task별 파라미터 격리를 위해 Task 이름을 지정하여 파라미터를 조회합니다.
        /// Task 이름이 비어있거나 등록되지 않은 경우 빈 TaskParams를 반환합니다.
        /// </details>
        /// <param name="taskName">파라미터를 조회할 Task 이름</param>
        /// <returns>지정된 Task의 파라미터 (없으면 빈 TaskParams)</returns>
        virtual TaskParams GetTaskParams(const std::string& taskName) const = 0;

        /// <summary>
        /// 실행 파라미터 조회 (문자열)
        /// </summary>
        virtual std::string GetExecutionParam(const std::string& key) const = 0;

        /// <summary>
        /// 실행 파라미터 설정 (문자열)
        /// </summary>
        virtual void SetExecutionParam(const std::string& key, const std::string& value) = 0;

        /// <summary>
        /// VisionPositions 목록을 조회합니다.
        /// </summary>
        virtual std::vector<VisionPosition> GetVisionPositions() const = 0;

        /// <summary>
        /// 마지막 VisionPosition을 조회합니다.
        /// </summary>
        virtual bool PeekVisionPosition(VisionPosition& outPos) const = 0;

    };
} // namespace VMF
