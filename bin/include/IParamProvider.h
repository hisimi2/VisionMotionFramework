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
     * @note Task가 다양해질 것을 대비하여 Task별 파라미터 구조체 접근을 제공합니다.
     */
    class IParamProvider
    {
    public:
        virtual ~IParamProvider() = default;

        /// <summary>
        /// Setup Task 전용 파라미터를 조회합니다.
        /// </summary>
        virtual SetPlate1PLVISetupParams GetSetupParams() const = 0;

        /// <summary>
        /// ExecuteScan Task 전용 파라미터를 조회합니다.
        /// </summary>
        virtual SetPlate1PLVIExecuteScanParams GetExecuteScanParams() const = 0;

        /// <summary>
        /// Finish Task 전용 파라미터를 조회합니다.
        /// </summary>
        virtual SetPlate1PLVIFinishParams GetFinishParams() const = 0;

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
        /// @deprecated 새로운 코드에서는 Task별 파라미터 구조체 전용 함수를 사용하세요.
        /// </summary>
        virtual std::string GetParam(const std::string& key) const = 0;

        /// <summary>
        /// 하위 호환성을 위한 타입 변환 파라미터 조회
        /// @deprecated 새로운 코드에서는 Task별 파라미터 구조체 전용 함수를 사용하세요.
        /// </summary>
        template <typename T>
        T GetParamAs(const std::string& key, const T& defaultValue = T()) const
        {
            std::string value = GetParam(key);
            if (value.empty())
                return defaultValue;

            std::istringstream iss(value);
            T converted;
            iss >> converted;
            if (iss.fail())
                return defaultValue;
            return converted;
        }
    };
} // namespace VMF_PLUGIN
