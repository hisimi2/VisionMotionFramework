#pragma once

#include "IDataRepository.h"
#include <mutex>

namespace VMF
{
    class VMF_API FileDataRepository : public IDataRepository
    {
    public:
        explicit FileDataRepository(const std::string& basePath);
        ~FileDataRepository() override;

        // 핵심 메서드
        StorageError SaveParam(const std::string& recipe, const std::string& name, const std::string& value) override;
        StorageError LoadParam(const std::string& recipe, const std::string& name, std::string& outValue) override;
        StorageError SaveImage(const std::string& contextTag, const std::vector<uint8_t>& imageData, std::string& outPath) override;
        StorageError Initialize() override;
        StorageError Shutdown() override;

        // 검사 실행 이력 관리
        StorageError CreateInspectionRun(InspectionType type, const std::string& runId,
            const std::string& paramsJson, int& outDbRunId) override;
        StorageError UpdateInspectionRunStatus(int dbRunId, const std::string& status,
            const std::string& resultJson, int errorCode = -1) override;
        StorageError LoadInspectionRun(int dbRunId, std::string& outTypeCode, std::string& outRunId,
            std::string& outStatus, std::string& outResultJson, int& outErrorCode) override;

        // 검사 결과 상세 저장/로드
        StorageError SaveInspectionResult(int dbRunId, const std::string& resultType,
            int resultIndex, const std::string& resultJson) override;
        StorageError LoadInspectionResults(int dbRunId, std::vector<InspectionResultItem>& outResults) override;

        // 시퀀스 실행 이력
        StorageError CreateSequenceRun(const std::string& sequenceName, const std::string& paramsJson, int& outRunId) override;
        StorageError UpdateSequenceRunStatus(int runId, const std::string& status,
            const std::string& resultSummaryJson) override;

        // Z-포커스 관련
        StorageError SaveZFocusPoint(int runId, double zPosition, double score,
            int sampleCount, const std::string& extraJson) override;
        StorageError SaveZFocusResult(int camIndex, int locationId, int pkgId, double newFocus) override;
        StorageError LoadInspInitPos(int camIndex, int locationId, int pkgId,
            double& outPosX, double& outPosY, double& outFocus) override;

        // 피커-캠 거리 관련
        StorageError SavePickerCamDistanceResult(int camIndex, int pkgId,
            double narrowX, double narrowY, double wideX, double wideY) override;
        StorageError LoadPickerCamDistance(int camIndex, int pkgId,
            double& outNarrowX, double& outNarrowY, double& outWideX, double& outWideY) override;

        // 캘리브레이션 위치 관련
        StorageError SaveCalibrationPosResult(int camIndex, int locationId, int pkgId,
            double posX, double posY) override;

        // 핸드 피치 관련
        StorageError SaveHandPitchResult(int handId, int pkgId, int row, int col,
            double narrowX, double narrowY, double wideX, double wideY) override;
        StorageError LoadHandPitch(int handId, int pkgId, int row, int col,
            double& outNarrowX, double& outNarrowY, double& outWideX, double& outWideY) override;

        // 티칭 결과 관련
        StorageError SaveTeachingResult(int handId, int locationId, int pkgId,
            double posX, double posY, double posZ) override;
        StorageError LoadTeachingResult(int handId, int locationId, int pkgId, int dateoffset,
            double& outPosX, double& outPosY, double& outPosZ) override;

        // 그룹 매핑 관련
        StorageError LoadHandCamGroup(int handId, std::vector<int>& outCamIds) override;
        StorageError LoadCamLocationGroup(int camIndex, std::vector<int>& outLocateIds) override;
        StorageError LoadLocationIdByName(const std::string& locateName, int& outLocationId) override;

        // 검사 위치 마스터
        StorageError LoadInspectionLocation(int locationCode, std::string& outLocationName,
            std::string& outLocationType) override;
        StorageError LoadAllInspectionLocations(std::vector<InspectionLocationInfo>& outLocations) override;

        // Socket 마스터
        StorageError LoadSocketMaster(int socketNo, int& outSocketType, int& outStationNo) override;
        StorageError LoadAllSocketMasters(std::vector<SocketMasterInfo>& outSockets) override;

    private:
        std::string basePath_;
        mutable std::mutex mutex_;
    };

} // namespace VMF

