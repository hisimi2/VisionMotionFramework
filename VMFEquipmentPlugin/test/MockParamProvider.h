#pragma once
#include "IParamProvider.h"
#include <map>
#include <string>

namespace VMF_PLUGIN
{
    /**
     * @brief 단위 테스트용 Mock IParamProvider 구현
     * 
     * Task의 단위 테스트 시 실제 Context 대신 이 Mock을 주입하여
     * 파라미터 기반 로직을 테스트할 수 있습니다.
     * 
     * @usage 예시:
     *   MockParamProvider mockProvider;
     *   mockProvider.SetupParams.timeoutMoveMs = 5000;
     *   mockProvider.ExecuteScanParams.scanEndY = 150.0;
     *   
     *   SetPlate1PLVIExecuteScan task;
     *   VMF::Context ctx;  // 또는 MockContext
     *   task.OnInitialize(ctx);  // 실제 Context 사용 시
     *   
     *   // 또는 직접 IParamProvider를 사용하는 테스트 메서드 추가 시:
     *   task.TestInitialize(mockProvider);
     */
    class MockParamProvider : public IParamProvider
    {
    public:
        SetPlate1PLVISetupParams         SetupParams;
        SetPlate1PLVIExecuteScanParams  ExecuteScanParams;
        SetPlate1PLVIFinishParams       FinishParams;
        std::vector<VisionPosition>     VisionPositions;
        std::map<std::string, std::string> StringParams;

        MockParamProvider()
        {
            // 기본값으로 초기화
            SetupParams.timeoutMoveMs = 7000;
            SetupParams.triggerIntervalMm = 1.8;
            
            ExecuteScanParams.timeoutMoveMs = 7000;
            ExecuteScanParams.timeoutResultMs = 10000;
            ExecuteScanParams.scanEndY = 200.0;
            
            FinishParams.timeoutMoveMs = 7000;
        }

        // IParamProvider 인터페이스 구현
        SetPlate1PLVISetupParams GetSetupParams() const override
        {
            return SetupParams;
        }

        SetPlate1PLVIExecuteScanParams GetExecuteScanParams() const override
        {
            return ExecuteScanParams;
        }

        SetPlate1PLVIFinishParams GetFinishParams() const override
        {
            return FinishParams;
        }

        std::vector<VisionPosition> GetVisionPositions() const override
        {
            return VisionPositions;
        }

        bool PeekVisionPosition(VisionPosition& outPos) const override
        {
            if (VisionPositions.empty())
                return false;
            outPos = VisionPositions.back();
            return true;
        }

        std::string GetParam(const std::string& key) const override
        {
            auto it = StringParams.find(key);
            if (it != StringParams.end())
                return it->second;
            return "";
        }
    };
} // namespace VMF_PLUGIN