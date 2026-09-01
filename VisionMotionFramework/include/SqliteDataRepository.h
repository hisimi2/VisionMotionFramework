#pragma once

#include "IDataRepository.h"
#include "IDataRepositoryExtension.h"
#include <mutex> // boost::mutex 대신 C++ 표준 라이브러리 사용

struct sqlite3; // forward declaration for sqlite3*

namespace VMF
{

class VMF_API SqliteDataRepository : public IDataRepository, public IDataRepositoryExtension
{
public:
    // dbFilePath: sqlite 파일 경로. imageBasePath: 이미지 파일을 저장할 디렉터리
    SqliteDataRepository(const std::string& dbFilePath, const std::string& imageBasePath);
    
    ~SqliteDataRepository() override;

    // ── IDataRepository (핵심) ──
    StorageError SaveParam(const std::string& recipe, const std::string& name, const std::string& value) override;
    StorageError LoadParam(const std::string& recipe, const std::string& name, std::string& outValue) override;
    StorageError SaveImage(const std::string& contextTag, const std::vector<uint8_t>& imageData, std::string& outPath) override;
    StorageError SaveSequenceRun(const std::string& sequenceName, const std::string& summary) override;
    StorageError Initialize() override;
    StorageError Shutdown() override;

    // ── IDataRepositoryExtension (확장) ──
    StorageError CreateSequenceRun(const std::string& sequenceName, const std::string& paramsJson, int& outRunId) override;
    StorageError SaveZFocusPoint(int runId, double zPosition, double score, int sampleCount, const std::string& extraJson) override;
    StorageError SaveZFocusResult(int camIndex, int locationId, int pkgId, double newFocus) override;
    StorageError SavePickerCamDistanceResult(int camIndex, int pkgId, double narrowX, double narrowY, double wideX, double wideY) override;
    StorageError SaveCalibrationPosResult(int camIndex, int locationId, int pkgId, double posX, double posY) override;
    StorageError SaveHandPitchResult(int handId, int pkgId, int col, int row, double narrowX, double narrowY, double wideX, double wideY) override;
    StorageError SaveTeachingResult(int handId, int locationId, int pkgId, double posX, double posY, double posZ) override;
    StorageError LoadInspInitPos(int camIndex, int locationId, int pkgId, double& posX, double& posY, double& focus) override;
    StorageError LoadPickerCamDistance(int camIndex, int pkgId, double& narrowX, double& narrowY, double& wideX, double& wideY) override;
    StorageError LoadHandPitch(int handId, int pkgId, int row, int col, double& narrowX, double& narrowY, double& wideX, double& wideY) override;
    StorageError LoadTeachingResult(int handId, int locationId, int pkgId, int dateoffset, double& posX, double& posY, double& posZ) override;
    StorageError LoadHandCamGroup(int handId, std::vector<int>& camIds) override;
    StorageError LoadCamLocationGroup(int camIndex, std::vector<int>& locateIds) override;
    StorageError LoadLocationIdByName(const std::string& locateName, int& locationId) override;
    StorageError UpdateSequenceRunStatus(int runId, const std::string& status, const std::string& resultSummaryJson) override;

private:
    // helpers
    StorageError executeSimple(const char* sql);
    std::string makeImageFilename(const std::string& tag);

private:
    std::string dbPath_;
    std::string imageBasePath_;
    sqlite3* db_;
    
    std::mutex mutex_;
    bool initialized_;
};

} // namespace VMF
