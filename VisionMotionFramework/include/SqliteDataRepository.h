#pragma once

#include "IDataRepository.h"
#include <mutex> // boost::mutex 대신 C++ 표준 라이브러리 사용

struct sqlite3; // forward declaration for sqlite3*

namespace VMF
{

class VMF_API SqliteDataRepository : public IDataRepository
{
public:
    // dbFilePath: sqlite 파일 경로. imageBasePath: 이미지 파일을 저장할 디렉터리
    SqliteDataRepository(const std::string& dbFilePath, const std::string& imageBasePath);
    
    ~SqliteDataRepository() override;

    // IDataRepository 인터페이스 구현
    StorageError SaveParam(const std::string& recipe, const std::string& name, const std::string& value) override;
    StorageError LoadParam(const std::string& recipe, const std::string& name, std::string& outValue) override;
    StorageError SaveImage(const std::string& contextTag, const std::vector<uint8_t>& imageData, std::string& outPath) override;
    StorageError SaveSequenceRun(const std::string& sequenceName, const std::string& summary) override;
    StorageError Initialize() override;
    StorageError Shutdown() override;

    // --- 새로 추가한 구조화된 저장 API ---
    StorageError CreateSequenceRun(const std::string& sequenceName, const std::string& paramsJson, int& outRunId) override;
    StorageError SaveZFocusPoint(int runId, double zPosition, double score, int sampleCount, const std::string& extraJson) override;


    //////////////검사 결과 저장//////////////////
    StorageError SaveZFocusResult(int camIndex, int locationId, int pkgId, double newFocus) override;


	// Picker-Cam 거리 값 저장
	StorageError SavePickerCamDistanceResult(int camIndex, int pkgId, double narrowX, double narrowY, double wideX, double wideY) override;

	// 검사 위치 정보 저장
	StorageError SaveCalibrationPosResult(int camIndex, int locationId, int pkgId, double posX, double posY) override;

	// 핸드 피치 정보 저장
	StorageError SaveHandPitchResult(
		int handId,
		int pkgId,
		int col,
		int row,
		double narrowX,
		double narrowY,
		double wideX,
		double wideY) override;

	// Teaching 검사 결과 저장
	StorageError SaveTeachingResult(int handId, int locationId, int pkgId, double posX, double posY, double posZ) override;


	//////////////검사 결과 로드//////////////////
	// InspInitPos 로드 (초기 위치 + Focus)
	StorageError LoadInspInitPos(
		int camIndex,
		int locationId,
		int pkgId,
		double& posX,
		double& posY,
		double& focus) override;


	// Picker-Cam 거리 로드
	StorageError LoadPickerCamDistance(
		int camIndex,
		int pkgId,
        double& narrowX,
        double& narrowY,
        double& wideX,
        double& wideY) override;


	// HandPitch 로드 (특정 row/col)
	StorageError LoadHandPitch(
		int handId,
		int pkgId,
		int row,
		int col,
		double& narrowX,
		double& narrowY,
		double& wideX,
		double& wideY) override;


	// Teaching 결과 로드
	StorageError LoadTeachingResult(
		int handId,
		int locationId,
		int pkgId,
		int dateoffset,
		double& posX,
		double& posY,
		double& posZ) override;

	StorageError LoadHandCamGroup(
		int handId,
		std::vector<int>& camIds) override;

    StorageError LoadCamLocationGroup(
        int camIndex,
        std::vector<int>& locateIds) override;

	StorageError LoadLocationIdByName(
		const std::string& locateName,
		int& locationId) override;

    // 추가: sequence_runs 상태 업데이트
    StorageError UpdateSequenceRunStatus(int runId, const std::string& status, const std::string& resultSummaryJson) override;

private:
    // helpers
    StorageError executeSimple(const char* sql);
    std::string makeImageFilename(const std::string& tag);

private:
    std::string dbPath_;
    std::string imageBasePath_;
    sqlite3* db_;
    
    // boost::mutex -> std::mutex 교체
    std::mutex mutex_;
    bool initialized_;
};

} // namespace VMF
