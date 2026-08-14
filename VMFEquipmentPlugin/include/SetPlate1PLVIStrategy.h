#pragma once

#include "DefaultSetupStrategy.h"
#include "VMFEquipmentPluginExport.h"
#include "Types.h"  // TaskParams 사용을 위해 추가
#include "IParamProvider.h"

namespace VMF_PLUGIN
{
    // 전방 선언만 필요한 클래스들
    class VisionPlviProcessor;
    class SetPlate1PLVISequenceBuilder;
    class SqliteDataRepository;

    /**
     * @brief SetPlate1 PLVI 측정 전략
     * 
     * SetPlate1 기구의 PLVI(Pattern Learning Vision Inspection) 측정을 위한 전략을 정의합니다.
     * 
     * @details
     * 이 전략은 PLVI 측정에 필요한 파라미터 설정, 데이터베이스 관리, 시퀀스 빌더 생성을 담당합니다.
     * 
     * @note 책임 분리:
     *   - Strategy: PLVI 측정에 필요한 파라미터 정의 (무엇을 설정할지)
     *   - Builder: Strategy로부터 받은 파라미터를 Context에 적용 (어떻게 설정할지)
     *   - Task: Context에서 파라미터를 조회하여 실행
     * 
     * ┌─────────────────────────────────────────────────────────────────┐
     * │ 관련 클래스                                                     │
     * ├─────────────────────────────────────────────────────────────────┤
     * │ SetPlate1PLVISequenceBuilder  - PLVI 측정 시퀀스 빌더          │
     * │ SetPlate1PLVISetup              - 측정 준비 Task              │
     * │ SetPlate1PLVIExecuteScan        - 측정 실행 Task              │
     * │ SetPlate1PLVIFinish             - 측정 종료 Task              │
     * └─────────────────────────────────────────────────────────────────┘
     * 
     * @note 총 3개의 Task로 구성된 시퀀스를 생성합니다.
     */
    class VMF_PLUGIN_API SetPlate1PLVIStrategy : public VMF::DefaultSetupStrategy
    {
    public:
        std::string GetSequenceName() const override;
        VMF::DataRepositoryPtr CreateRepository() override;
        VMF::VisionProcessorPtr CreateVisionProcessor() override;
        VMF::SequenceBuilderPtr CreateBuilder() override;
        
        /// <summary>
        /// 전략 레벨의 파라미터를 설정합니다.
        /// </summary>
        /// <details>
        /// 이 메서드는 전략(Strategy) 레벨에서 필요한 파라미터를 설정합니다.
        /// 주로 Repository에 영구 저장되어야 하는 파라미터(예: Device 정보, PLVI 설정값 등)를 
        /// 처리하는 데 사용됩니다.
        /// 
        /// 실행 시 필요한 런타임 파라미터(TIMEOUT, SCAN_END_Y 등)는 
        /// SetPlate1PLVISequenceBuilder::ConfigureContext()에서 Context를 통해 설정합니다.
        /// 
        /// 현재 구현에서는 실행 파라미터를 설정하지 않으며, 인터페이스 준수를 위해 유지됩니다.
        /// Repository 저장이 필요한 파라미터가 있다면 이 메서드에서 처리해야 합니다.
        /// </details>
        /// <param name="ctx">VisionContext 포인터</param>
        void ConfigureParams(VMF::VisionContextPtr ctx) override;
        
        VMF::StringMap GetVisionParams(const std::string& presetName) const override;

        /// <summary>
        /// PLVI 측정 시퀀스에 필요한 기본 Task 파라미터를 반환합니다.
        /// </summary>
        /// <details>
        /// 이 메서드는 Strategy가 PLVI 측정에 필요한 기본 파라미터 값을 정의합니다.
        /// Builder는 이 메서드를 통해 기본 파라미터를 받아 Context에 설정합니다.
        /// 
        /// 기본 파라미터는 다음과 같습니다:
        /// - Setup Task: timeoutMoveMs, triggerIntervalMm, startPos
        /// - ExecuteScan Task: timeoutMoveMs, timeoutResultMs, scanEndY
        /// - Finish Task: timeoutMoveMs
        /// - 공통 Vision 파라미터: HAND_ID, PKG_ID, DEVICE_INFO_* 등 (executionParams에 저장)
        /// 
        /// @note 파라미터 값이 변경될 경우 이 메서드만 수정하면 됩니다.
        /// </details>
        /// <returns>PLVI 측정용 기본 TaskParams</returns>
        VMF::TaskParams GetDefaultTaskParams() const;
    };
}
