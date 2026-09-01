#pragma once

#include "VisionMotionFramework/SequenceBuilderBase.h"
#include "PlugInAPI.h"

#include <memory>

namespace VMF_PLUGIN
{
    /// <summary>
    /// SetPlate1 PLVI 시퀀스를 조립하는 빌더 클래스입니다.
    /// </summary>
    /// <remarks>
    /// 역할:
    /// - Task 3종(SetPlate1PLVISetup, SetPlate1PLVIExecuteScan, SetPlate1PLVIFinish)을
    ///   순서대로 시퀀스에 추가합니다.
    /// - 파라미터 설정은 하지 않습니다 (Strategy::ConfigureContext에서 처리).
    /// 
    /// 참고:
    /// - 이 빌더는 SequenceBuilderBase::ConfigureContext()를 오버라이드하지 않습니다.
    ///   파라미터 설정은 SetPlate1PLVIStrategy::ConfigureContext()에서 직접 수행합니다.
    /// - Task 조립만 담당하므로 불필요한 헤더 포함을 최소화합니다.
    /// </remarks>
    class VMF_PLUGIN_API SetPlate1PLVISequenceBuilder : public VMF::SequenceBuilderBase
    {
    protected:
        /// <summary>
        /// SetPlate1 PLVI 시퀀스를 생성합니다.
        /// </summary>
        /// <param name="sequenceName">시퀀스 이름 (예: "SetPlate1PLVI")</param>
        /// <returns>조립된 시퀀스 객체의 소유권</returns>
        VMF::SequencePtr BuildSequence(const std::string& sequenceName) override;
    };
}
