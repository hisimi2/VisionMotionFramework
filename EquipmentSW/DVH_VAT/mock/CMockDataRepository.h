#pragma once

#include "IDataRepository.h"

#include <map>
#include <boost/thread/mutex.hpp>
#include <string>
#include <vector>

namespace DVH_VAT
{
    // IDataRepository의 단순 모의 구현체
    // 테스트 중 저장/조회 동작을 시뮬레이션합니다.
    class CMockDataRepository : public IDataRepository
    {
    public:
        CMockDataRepository();
        virtual ~CMockDataRepository();

        // IDataRepository 구현
        virtual StorageError SaveParam(const std::string& recipe, const std::string& name, const std::string& value);
        virtual StorageError LoadParam(const std::string& recipe, const std::string& name, std::string& outValue);

        virtual StorageError SaveImage(const std::string& contextTag, const std::vector<uint8_t>& imageData, std::string& outPath);

        virtual StorageError SaveSequenceRun(const std::string& sequenceName, const std::string& summary);

        virtual StorageError Initialize();
        virtual StorageError Shutdown();

        virtual StorageError CreateSequenceRun(const std::string& sequenceName, const std::string& paramsJson, int& outRunId);
        virtual StorageError SaveZFocusPoint(int runId, double zPosition, double score, int sampleCount, const std::string& extraJson);

        virtual StorageError SaveZFocusResult(int camIndex, int locationId, int pkgId, double newFocus);
        virtual StorageError SavePickerCamDistanceResult(int camIndex, int pkgId, double narrowX, double narrowY, double wideX, double wideY);
        virtual StorageError SaveCalibrationPosResult(int camIndex, int locationId, int pkgId, double posX, double posY);
        virtual StorageError UpdateSequenceRunStatus(int runId, const std::string& status, const std::string& resultSummaryJson);
        virtual StorageError SaveHandPitchResult(int handId, int pkgId, int col, int row, double narrowX, double narrowY, double wideX, double wideY);
        virtual StorageError SaveTeachingResult(int handId, int locationId, int pkgId, double posX, double posY, double posZ);

        virtual StorageError LoadInspInitPos(int camIndex, int locationId, int pkgId, double& posX, double& posY, double& focus);

        virtual StorageError LoadPickerCamDistance(int camIndex, int pkgId, double& narrowX, double& narrowY, double& wideX, double& wideY);

        virtual StorageError LoadHandPitch(int handId, int pkgId, int row, int col, double& narrowX, double& narrowY, double& wideX, double& wideY);

        virtual StorageError LoadTeachingResult(int handId, int locationId, int pkgId, int dateoffset, double& posX, double& posY, double& posZ);

        virtual StorageError LoadHandCamGroup(int handId, std::vector<int>& camIds);

        virtual StorageError LoadCamLocationGroup(int camIndex, std::vector<int>& locateIds);

        virtual StorageError LoadLocationIdByName(const std::string& locateName, int& locationId);

        // 테스트 헬퍼
        void ClearAll();

    private:
        boost::mutex m_mutex;

        // 간단한 키-값 저장소: recipe:name -> value
        std::map<std::string, std::string> m_params;

        // 이미지 저장 시 contextTag -> path
        std::map<std::string, std::string> m_images;

        // sequence run id 생성기 (인스턴스 멤버로 명확히 선언)
        int m_nextRunId;
    };
} // namespace DVH_VAT

