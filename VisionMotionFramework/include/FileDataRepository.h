#pragma once

#include "IDataRepository.h"
#include "IDataRepositoryExtension.h"
#include <mutex> 

namespace VMF
{
    class VMF_API FileDataRepository : public IDataRepository, public IDataRepositoryExtension
    {
    public:
        /**
         * @brief 생성자.
         * @param basePath 데이터 저장소의 기본 경로.
         */
        explicit FileDataRepository(const std::string& basePath);

        /**
         * @brief 소멸자.
         */
        ~FileDataRepository() override;

        // ── IDataRepository (핵심) ──
        StorageError SaveParam(const std::string& recipe, const std::string& name, const std::string& value) override;
        StorageError LoadParam(const std::string& recipe, const std::string& name, std::string& outValue) override;
        StorageError SaveImage(const std::string& contextTag, const std::vector<uint8_t>& imageData, std::string& outPath) override;
        StorageError SaveSequenceRun(const std::string& sequenceName, const std::string& summary) override;
        StorageError Initialize() override;
        StorageError Shutdown() override;

        /* --- IDataRepositoryExtension (확장) --- */
        // sequence_runs 레코드 생성
        StorageError CreateSequenceRun(const std::string& sequenceName, const std::string& paramsJson, int& outRunId) override;
        StorageError SaveZFocusPoint(int runId, double zPosition, double score, int sampleCount, const std::string& extraJson) override;
        StorageError SaveZFocusResult(int camIndex, int locationId, int pkgId, double newFocus) override;
        StorageError SavePickerCamDistanceResult(int camIndex, int pkgId, double narrowX, double narrowY, double wideX, double wideY) override;
        StorageError SaveCalibrationPosResult(int camIndex, int locationId, int pkgId, double posX, double posY) override;
        StorageError SaveHandPitchResult(int handId, int pkgId, int col, int row, double narrowX, double narrowY, double wideX, double wideY) override;
        StorageError SaveTeachingResult(int handId, int locationId, int pkgId, double posX, double posY, double posZ) override;

        // 검사 결과 로드
        StorageError LoadInspInitPos(int camIndex, int locationId, int pkgId, double& posX, double& posY, double& focus) override;
        StorageError LoadPickerCamDistance(int camIndex, int pkgId, double& narrowX, double& narrowY, double& wideX, double& wideY) override;
        StorageError LoadHandPitch(int handId, int pkgId, int row, int col, double& narrowX, double& narrowY, double& wideX, double& wideY) override;
        StorageError LoadTeachingResult(int handId, int locationId, int pkgId, int dateoffset, double& posX, double& posY, double& posZ) override;
        StorageError LoadHandCamGroup(int handId, std::vector<int>& camIds) override;
        StorageError LoadCamLocationGroup(int camIndex, std::vector<int>& locateIds) override;
        StorageError LoadLocationIdByName(const std::string& locateName, int& locationId) override;
        StorageError UpdateSequenceRunStatus(int runId, const std::string& status, const std::string& resultSummaryJson) override;

    private:
        std::string basePath_;
        mutable std::mutex mutex_;
    };

}; // namespace VMF
