#pragma once

#include "IDataRepository.h"
#include <memory>

namespace VMF 
{
    /**
     * @class AsyncDataRepository
     * @brief IDataRepository 구현체를 비동기 큐로 래핑하여 Pimpl 패턴을 적용합니다.
     */
    class VMF_API AsyncDataRepository : public IDataRepository 
    {
    public:
        AsyncDataRepository(IDataRepository* inner, bool ownInner);
        
        ~AsyncDataRepository() override;

        // IDataRepository 인터페이스
        StorageError SaveParam(const std::string& recipe, const std::string& name, const std::string& value) override;
        StorageError LoadParam(const std::string& recipe, const std::string& name, std::string& outValue) override;
        StorageError SaveImage(const std::string& contextTag, const std::vector<uint8_t>& imageData, std::string& outPath) override;
        StorageError SaveSequenceRun(const std::string& sequenceName, const std::string& summary) override;
        StorageError Initialize() override;
        StorageError Shutdown() override;
        StorageError CreateSequenceRun(const std::string& sequenceName, const std::string& paramsJson, int& outRunId) override;

        // 비동기 처리 대상
        StorageError SaveZFocusPoint(int runId, double zPosition, double score, int sampleCount, const std::string& extraJson) override;
        StorageError SaveZFocusResult(int camIndex, int locationId, int pkgId, double newFocus) override;
        StorageError UpdateSequenceRunStatus(int runId, const std::string& status, const std::string& resultSummaryJson) override;

        // Picker-Cam 거리 값 저장
        StorageError SavePickerCamDistanceResult(int camIndex, int pkgId, double narrowX, double narrowY, double wideX, double wideY) override;

        // 검사 위치 정보 저장
        StorageError SaveCalibrationPosResult(int camIndex, int locationId, int pkgId, double posX, double posY) override;

        // 핸드 피치 정보 저장
        StorageError SaveHandPitchResult(int handId, int pkgId, int col, int row, double narrowX, double narrowY, double wideX, double wideY) override;

        StorageError SaveTeachingResult(int handId, int locationId, int pkgId, double posX, double posY, double posZ) override;

        //////////////검사 결과 로드//////////////////
        StorageError LoadInspInitPos(int camIndex, int locationId, int pkgId, double& posX, double& posY, double& focus);
        StorageError LoadPickerCamDistance(int camIndex, int pkgId, double& narrowRight, double& narrowLeft, double& wideRight, double& wideLeft);
        StorageError LoadHandPitch(int handId, int pkgId, int row, int col, double& narrowX, double& narrowY, double& wideX, double& wideY);
        StorageError LoadTeachingResult(int handId, int locationId, int pkgId, int dateoffset, double& posX, double& posY, double& posZ);
        StorageError LoadHandCamGroup(int handId, std::vector<int>& camIds);
        StorageError LoadCamLocationGroup(int camIndex, std::vector<int>& locateIds);
        StorageError LoadLocationIdByName(const std::string& locateName, int& locationId);

    private:
        // Pimpl 관용구: 구현부 구조체 전방 선언
        struct Impl;
    
        // C++14: 수동 메모리 관리(delete)에서 발생하는 누수를 막기 위해 std::unique_ptr 사용
        std::unique_ptr<Impl> m_pImpl;
    };

} // namespace VMF
