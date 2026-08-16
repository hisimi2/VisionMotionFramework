#pragma once

#include "DefaultSetupStrategy.h"
#include "VMFEquipmentPluginExport.h"
#include "Types.h"  // TaskParams 사용을 위해 추가
#include "IParamProvider.h"

#include <memory>  // std::enable_shared_from_this

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
     *   - Builder: Strategy로부터 받은 파라미터를 Context에 적용 (어떻게 설정할지) [4단계: 제거 가능]
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
     * 
     * std::enable_shared_from_this는 DLL 경계 문제로 인해 베이스 클래스(DefaultSetupStrategy)가 아닌
     * 파생 클래스(같은 DLL 내에서 shared_from_this()를 호출하는 쪽)에서 상속받습니다.
     */
    class VMF_PLUGIN_API SetPlate1PLVIStrategy : public VMF::DefaultSetupStrategy, public std::enable_shared_from_this<SetPlate1PLVIStrategy>
    {
    public:
        std::string GetSequenceName() const override;
        VMF::DataRepositoryPtr CreateRepository() override;
        VMF::VisionProcessorPtr CreateVisionProcessor() override;
        
        /// <summary>
        /// 시퀀스 빌더를 생성합니다. (하위 호환성 유지)
        /// </summary>
        /// <details>
        /// 기존 코드와의 호환성을 위해 유지됩니다.
        /// Builder 없이 직접 Context에 파라미터를 설정하려면 ConfigureContext()를 사용하세요.
        /// </details>
        /// <returns>생성된 SequenceBuilderPtr</returns>
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
        /// ConfigureContext()에서 Context를 통해 설정합니다.
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

        /// <summary>
        /// Setup Task 전용 기본 파라미터를 반환합니다.
        /// </summary>
        /// <details>
        /// Setup Task에 필요한 기본 파라미터만 반환합니다.
        /// GetDefaultTaskParams()에서 Setup 관련 파라미터를 추출하여 사용합니다.
        /// </details>
        /// <returns>Setup Task용 기본 TaskParams</returns>
        VMF::TaskParams GetDefaultSetupParams() const;

        /// <summary>
        /// ExecuteScan Task 전용 기본 파라미터를 반환합니다.
        /// </summary>
        /// <details>
        /// ExecuteScan Task에 필요한 기본 파라미터만 반환합니다.
        /// GetDefaultTaskParams()에서 ExecuteScan 관련 파라미터를 추출하여 사용합니다.
        /// </details>
        /// <returns>ExecuteScan Task용 기본 TaskParams</returns>
        VMF::TaskParams GetDefaultExecuteScanParams() const;

        /// <summary>
        /// Finish Task 전용 기본 파라미터를 반환합니다.
        /// </summary>
        /// <details>
        /// Finish Task에 필요한 기본 파라미터만 반환합니다.
        /// GetDefaultTaskParams()에서 Finish 관련 파라미터를 추출하여 사용합니다.
        /// </details>
        /// <returns>Finish Task용 기본 TaskParams</returns>
        VMF::TaskParams GetDefaultFinishParams() const;

        /// <summary>
        /// 공통 Vision 파라미터 기본값을 반환합니다.
        /// </summary>
        /// <details>
        /// 모든 Task에서 공통으로 사용하는 Vision 파라미터를 반환합니다.
        /// GetDefaultTaskParams()에서 Vision 관련 파라미터를 추출하여 사용합니다.
        /// </details>
        /// <returns>공통 Vision 파라미터가 포함된 TaskParams</returns>
        VMF::TaskParams GetDefaultVisionParams() const;

        /// <summary>
        /// Task별 파라미터를 Context에 설정합니다. (Task별 격리)
        /// </summary>
        /// <details>
        /// 이 메서드는 Task별로 분리된 파라미터를 Context에 설정합니다.
        /// Task 간 파라미터 키 충돌을 방지하고, Task별 파라미터 격리를 위해 사용됩니다.
        /// 
        /// 각 Task는 자신의 이름으로 파라미터를 조회할 수 있습니다:
        /// - "Task_PLVI_Setup": Setup Task 파라미터
        /// - "Task_PLVI_ExecuteScan": ExecuteScan Task 파라미터
        /// - "Task_PLVI_Finish": Finish Task 파라미터
        /// 
        /// @note 3단계 리팩토링(Task별 맵 분리) 적용 시 사용됩니다.
        /// </details>
        /// <param name="ctx">Context 참조</param>
        void SetTaskParamsByTask(VMF::Context& ctx) const;

/// <summary>
        /// Builder 없이 Context에 파라미터를 직접 설정합니다. (4단계 리팩토링)
        /// </summary>
        /// <details>
        /// Builder 중간 단계 없이 Strategy가 직접 Context에 파라미터를 설정합니다.
        /// SetTaskParamsByTask()를 호출하여 Task별 파라미터를 Context에 설정합니다.
        /// 
        /// 이 메서드는 4단계 리팩토링(Strategy → Context 직접 전달) 적용 시 사용됩니다.
        /// Builder를 사용하지 않는 경우 이 메서드를 통해 Context에 파라미터를 설정합니다.
        /// </details>
        /// <param name="ctx">Context 참조</param>
        void ConfigureContext(VMF::Context& ctx) override;

        /// <summary>
        /// Task별 파라미터 설정 헬퍼 메서드
        /// </summary>
        /// <details>
        /// 지정된 Task 이름과 파라미터로 Context에 Task별 파라미터를 설정합니다.
        /// SetTaskParamsByTask()에서 반복 패턴을 추상화하기 위해 사용됩니다.
        /// </details>
        /// <param name="ctx">Context 참조</param>
        /// <param name="taskName">파라미터를 설정할 Task 이름</param>
        /// <param name="params">설정할 TaskParams</param>
        void SetTaskParamsForTask(VMF::Context& ctx, const std::string& taskName, const VMF::TaskParams& params) const;
    };
}
