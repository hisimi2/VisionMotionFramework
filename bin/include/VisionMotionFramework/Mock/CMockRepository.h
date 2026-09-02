#pragma once

#include "IDataRepository.h"

#include <map>
#include <mutex>
#include <string>
#include <vector>

namespace VMF
{
    // IDataRepository의 단순 모의 구현체
    // 테스트 중 저장/조회 동작을 시뮬레이션합니다.
    class CMockRepository : public IDataRepository
    {
    public:
        CMockRepository();
        virtual ~CMockRepository();

        // IDataRepository 구현
        virtual StorageError SaveParam(const std::string& recipe, const std::string& name, const std::string& value);
        virtual StorageError LoadParam(const std::string& recipe, const std::string& name, std::string& outValue);

        virtual StorageError SaveImage(const std::string& contextTag, const std::vector<uint8_t>& imageData, std::string& outPath);

        virtual StorageError Initialize();
        virtual StorageError Shutdown();

        virtual StorageError CreateSequenceRun(const std::string& sequenceName, const std::string& paramsJson, int& outRunId);
        virtual StorageError UpdateSequenceRunStatus(int runId, const std::string& status, const std::string& resultSummaryJson);

        virtual StorageError SaveZFocusPoint(int runId, double zPosition, double score, int sampleCount, const std::string& extraJson);
        virtual StorageError SaveZFocusResult(int camIndex, int locationId, int pkgId, double newFocus);
        virtual StorageError SavePickerCamDistanceResult(int camIndex, int pkgId, double narrowX, double narrowY, double wideX, double wideY);
        virtual StorageError SaveCalibrationPosResult(int camIndex, int locationId, int pkgId, double posX, double posY);
        virtual StorageError SaveHandPitchResult(int handId, int pkgId, int col, int row, double narrowX, double narrowY, double wideX, double wideY);
        virtual StorageError SaveTeachingResult(int handId, int locationId, int pkgId, double posX, double posY, double posZ);

        virtual StorageError LoadInspInitPos(int camIndex, int locationId, int pkgId, double& posX, double& posY, double& focus);
        virtual StorageError LoadPickerCamDistance(int camIndex, int pkgId, double& narrowX, double& narrowY, double& wideX, double& wideY);
        virtual StorageError LoadHandPitch(int handId, int pkgId, int row, int col, double& narrowX, double& narrowY, double& wideX, double& wideY);
        virtual StorageError LoadTeachingResult(int handId, int locationId, int pkgId, int dateoffset, double& posX, double& posY, double& posZ);

        virtual StorageError LoadHandCamGroup(int handId, std::vector<int>& camIds);
        virtual StorageError LoadCamLocationGroup(int camIndex, std::vector<int>& locateIds);
        virtual StorageError LoadLocationIdByName(const std::string& locateName, int& outLocationId);

        virtual StorageError LoadInspectionLocation(int locationCode, std::string& outLocationName, std::string& outLocationType);
        virtual StorageError LoadAllInspectionLocations(std::vector<InspectionLocationInfo>& outLocations);
        virtual StorageError LoadSocketMaster(int socketNo, int& outSocketType, int& outStationNo);
        virtual StorageError LoadAllSocketMasters(std::vector<SocketMasterInfo>& outSockets);

        virtual StorageError CreateInspectionRun(InspectionType type, const std::string& runId, const std::string& paramsJson, int& outDbRunId);
        virtual StorageError SaveInspectionResult(int dbRunId, const std::string& resultType, int resultIndex, const std::string& resultJson);
        virtual StorageError LoadInspectionResults(int dbRunId, std::vector<InspectionResultItem>& outResults);
        virtual StorageError LoadInspectionRun(int dbRunId, std::string& outTypeCode, std::string& outRunId, std::string& outStatus, std::string& outResultJson, int& outErrorCode);

        // 테스트 헬퍼
        void ClearAll();

    private:
        std::mutex m_mutex;

        // 간단한 키-값 저장소: recipe:name -> value
        std::map<std::string, std::string> m_params;

        // 이미지 저장 시 contextTag -> path
        std::map<std::string, std::string> m_images;

        // sequence run id 생성기
        int m_nextRunId;
    };
} // namespace VMF

