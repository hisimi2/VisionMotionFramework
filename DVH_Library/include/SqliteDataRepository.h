#pragma once

#include "IDataRepository.h"

struct sqlite3; // forward declaration for sqlite3*

namespace DVH_VAT
{

class DVH_VAT_API SqliteDataRepository : public IDataRepository
{
public:
    // dbFilePath: sqlite 파일 경로. imageBasePath: 이미지 파일을 저장할 디렉터리
    SqliteDataRepository(const std::string& dbFilePath, const std::string& imageBasePath);
    virtual ~SqliteDataRepository();

    // IDataRepository 인터페이스 구현
    // [v100] override 키워드 제거 (VS2010 일부 버전 미지원 가능성 고려)
    virtual StorageError SaveParam(const std::string& recipe, const std::string& name, const std::string& value);
    virtual StorageError LoadParam(const std::string& recipe, const std::string& name, std::string& outValue);
    virtual StorageError SaveImage(const std::string& contextTag, const std::vector<uint8_t>& imageData, std::string& outPath);
    virtual StorageError SaveSequenceRun(const std::string& sequenceName, const std::string& summary);
    virtual StorageError Initialize(); // override 제거
    virtual StorageError Shutdown();   // override 제거

    // --- 새로 추가한 구조화된 저장 API ---
    virtual StorageError CreateSequenceRun(const std::string& sequenceName, const std::string& paramsJson, int& outRunId);
    virtual StorageError SaveZFocusPoint(int runId, double zPosition, double score, int sampleCount, const std::string& extraJson);




    //////////////검사 결과 저장//////////////////
    virtual StorageError SaveZFocusResult(int camIndex, int locationId, int pkgId, double newFocus);


	// Picker-Cam 거리 값 저장
	virtual StorageError SavePickerCamDistanceResult(int camIndex, int pkgId, double narrowX, double narrowY, double wideX, double wideY);

	// 검사 위치 정보 저장
	virtual StorageError SaveCalibrationPosResult(int camIndex, int locationId, int pkgId, double posX, double posY);

	// 핸드 피치 정보 저장
	virtual StorageError SaveHandPitchResult(
		int handId,
		int pkgId,
		int col,
		int row,
		double narrowX,
		double narrowY,
		double wideX,
		double wideY);

	// Teaching 검사 결과 저장
	virtual StorageError SaveTeachingResult(int handId, int locationId, int pkgId, double posX, double posY, double posZ);


	//////////////검사 결과 로드//////////////////
	// InspInitPos 로드 (초기 위치 + Focus)
	virtual StorageError LoadInspInitPos(
		int camIndex,
		int locationId,
		int pkgId,
		double& posX,
		double& posY,
		double& focus);


	// Picker-Cam 거리 로드
	virtual StorageError LoadPickerCamDistance(
		int camIndex,
		int pkgId,
        double& narrowX,
        double& narrowY,
        double& wideX,
        double& wideY);


	// HandPitch 로드 (특정 row/col)
	virtual StorageError LoadHandPitch(
		int handId,
		int pkgId,
		int row,
		int col,
		double& narrowX,
		double& narrowY,
		double& wideX,
		double& wideY);


	// Teaching 결과 로드
	virtual StorageError LoadTeachingResult(
		int handId,
		int locationId,
		int pkgId,
		int dateoffset,
		double& posX,
		double& posY,
		double& posZ);

	virtual StorageError LoadHandCamGroup(
		int handId,
		std::vector<int>& camIds);

    virtual StorageError LoadCamLocationGroup(
        int camIndex,
        std::vector<int>& locateIds);

	virtual StorageError LoadLocationIdByName(
		const std::string& locateName,
		int& locationId);

    // 추가: sequence_runs 상태 업데이트
    virtual StorageError UpdateSequenceRunStatus(int runId, const std::string& status, const std::string& resultSummaryJson);

private:
    // helpers
    StorageError executeSimple(const char* sql);
    std::string makeImageFilename(const std::string& tag);

private:
    std::string dbPath_;
    std::string imageBasePath_;
    sqlite3* db_;
    
    // [v100] boost::mutex (CompatUtils.h에 정의됨)
    boost::mutex mutex_;
    bool initialized_;
};

} // namespace DVH_VAT
