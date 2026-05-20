#include "stdafx.h"
#include "FileDataRepository.h"
#include "FileUtils.h"
#include "CompatUtils.h" // StorageError 정의
#include <fstream>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <iostream> 
#include <mutex> // std::lock_guard 사용을 위해 포함

namespace VMF {

FileDataRepository::FileDataRepository(const std::string& basePath)
    : basePath_(basePath) {}

FileDataRepository::~FileDataRepository() = default; 

StorageError FileDataRepository::SaveParam(const std::string& recipe, const std::string& name, const std::string& value) {
    // LockGuardType 대신 C++11 표준 std::lock_guard 사용
    std::lock_guard<std::mutex> lg(mutex_); 
    std::ofstream ofs(FileUtils::MakeParamPath(basePath_, recipe).c_str(), std::ios::app);
    if (!ofs) {
        std::cerr << "[FileDataRepository] Failed to open file for writing: " << FileUtils::MakeParamPath(basePath_, recipe) << std::endl;
        return StorageWriteFailed;
    }

    ofs << name << "=" << value << "\n";
    return StorageSuccess;
}

StorageError FileDataRepository::LoadParam(const std::string& recipe, const std::string& name, std::string& outValue) {
    std::lock_guard<std::mutex> lg(mutex_);
    std::ifstream ifs(FileUtils::MakeParamPath(basePath_, recipe).c_str());
    if (!ifs) {
        std::cerr << "[FileDataRepository] Failed to open file for reading: " << FileUtils::MakeParamPath(basePath_, recipe) << std::endl;
        return StorageFileNotFound;
    }

    std::string line;
    while (std::getline(ifs, line)) {
        size_t pos = line.find('=');
        if (pos != std::string::npos && line.substr(0, pos) == name) {
            outValue = line.substr(pos + 1);
            return StorageSuccess;
        }
    }
    return StorageFileNotFound;
}

StorageError FileDataRepository::SaveImage(const std::string& contextTag, const std::vector<uint8_t>& imageData, std::string& outPath) {
    std::lock_guard<std::mutex> lg(mutex_);
    outPath = FileUtils::MakeImagePath(basePath_, contextTag);
    std::ofstream ofs(outPath.c_str(), std::ios::binary);
    if (!ofs) {
        std::cerr << "[FileDataRepository] Failed to open file for writing: " << outPath << std::endl;
        return StorageWriteFailed;
    }

    if (!imageData.empty()) {
        ofs.write(reinterpret_cast<const char*>(&imageData[0]), static_cast<std::streamsize>(imageData.size()));
    }
    return StorageSuccess;
}

StorageError FileDataRepository::SaveSequenceRun(const std::string& sequenceName, const std::string& summary) {
    std::lock_guard<std::mutex> lg(mutex_);
    std::string logPath = FileUtils::JoinPath(basePath_, "sequence_runs.log");
    std::ofstream ofs(logPath.c_str(), std::ios::app);
    if (!ofs) {
        std::cerr << "[FileDataRepository] Failed to open log file: " << logPath << std::endl;
        return StorageWriteFailed;
    }

    ofs << FileUtils::MakeTimeTag() << " | " << sequenceName << " | " << summary << "\n";
    return StorageSuccess;
}

StorageError FileDataRepository::Initialize() {
    return StorageSuccess;
}

StorageError FileDataRepository::Shutdown() {
    return StorageSuccess;
}

// --- 구현: 추가 구조화 API (간단 파일 기반 구현) ---

StorageError FileDataRepository::CreateSequenceRun(const std::string& sequenceName, const std::string& paramsJson, int& outRunId)
{
    std::lock_guard<std::mutex> lg(mutex_);
    std::time_t t = std::time(nullptr); 
    int runId = static_cast<int>(t & 0x7FFFFFFF); // 간단 ID 생성

    std::ostringstream ss;
    ss << FileUtils::MakeTimeTag() << " | CREATE_RUN | id=" << runId << " | type=" << sequenceName << " | params=" << paramsJson << "\n";

    std::string logPath = FileUtils::JoinPath(basePath_, "sequence_runs.log");
    std::ofstream ofs(logPath.c_str(), std::ios::app);
    if (!ofs) {
        std::cerr << "[FileDataRepository] Failed to open sequence_runs.log for CreateSequenceRun\n";
        return StorageWriteFailed;
    }
    ofs << ss.str();
    outRunId = runId;
    return StorageSuccess;
}

StorageError FileDataRepository::SaveZFocusPoint(int runId, double zPosition, double score, int sampleCount, const std::string& extraJson)
{
    std::lock_guard<std::mutex> lg(mutex_);
    std::ostringstream ss;
    ss << FileUtils::MakeTimeTag() << " | ZPOINT | run=" << runId << " | z=" << zPosition << " | score=" << score
       << " | samples=" << sampleCount << " | extra=" << extraJson << "\n";

    std::string logPath = FileUtils::JoinPath(basePath_, "zfocus_points.log");
    std::ofstream ofs(logPath.c_str(), std::ios::app);
    if (!ofs) {
        std::cerr << "[FileDataRepository] Failed to open zfocus_points.log\n";
        return StorageWriteFailed;
    }
    ofs << ss.str();
    return StorageSuccess;
}

StorageError FileDataRepository::SaveZFocusResult(int camIndex, int locationId, int pkgId, double newFocus)
{
    std::lock_guard<std::mutex> lg(mutex_);
    std::ostringstream ss;
    ss << FileUtils::MakeTimeTag() << " | ZRESULT | camIndex=" << camIndex 
       << " | locationId=" << locationId << " | pkgId=" << pkgId << " | Focus=" << newFocus << "\n";

    std::string logPath = FileUtils::JoinPath(basePath_, "zfocus_results.log");
    std::ofstream ofs(logPath.c_str(), std::ios::app);
    if (!ofs) {
        std::cerr << "[FileDataRepository] Failed to open zfocus_results.log\n";
        return StorageWriteFailed;
    }
    ofs << ss.str();
    return StorageSuccess;
}

// Picker-Cam 거리 값 저장 (Stub)
StorageError FileDataRepository::SavePickerCamDistanceResult(int camIndex, int pkgId, double narrowX, double narrowY, double wideX, double wideY) {
	return StorageSuccess;
}

// 검사 위치 정보 저장 (Stub)
StorageError FileDataRepository::SaveCalibrationPosResult(int camIndex, int locationId, int pkgId, double posX, double posY) {
	return StorageSuccess;
}

// 핸드 피치 정보 저장 (Stub)
StorageError FileDataRepository::SaveHandPitchResult(
	int handId,
	int pkgId,
	int col,
	int row,
	double narrowX,
	double narrowY,
	double wideX,
	double wideY) {
	return StorageSuccess;
}

// 티칭 결과 저장 (Stub)
StorageError FileDataRepository::SaveTeachingResult(
	int handId,
	int locationId,
	int pkgId,
	double posX,
	double posY,
	double posZ) {
	return StorageSuccess;
}

//////////////////////////////////////////////////////////
// 검사 결과 로드 (Stub Version)
//////////////////////////////////////////////////////////

StorageError FileDataRepository::LoadInspInitPos(
	int camIndex,
	int locationId,
	int pkgId,
	double& posX,
	double& posY,
	double& focus)
{
	posX = 0.0;
	posY = 0.0;
	focus = 0.0;
	return StorageSuccess;
}


StorageError FileDataRepository::LoadPickerCamDistance(
	int camIndex,
	int pkgId,
	double& narrowRight,
	double& narrowLeft,
	double& wideRight,
	double& wideLeft)
{
	narrowRight = 0.0;
	narrowLeft = 0.0;
	wideRight = 0.0;
	wideLeft = 0.0;
	return StorageSuccess;
}


StorageError FileDataRepository::LoadHandPitch(
	int handId,
	int pkgId,
	int row,
	int col,
	double& narrowX,
	double& narrowY,
	double& wideX,
	double& wideY)
{
	narrowX = 0.0;
	narrowY = 0.0;
	wideX = 0.0;
	wideY = 0.0;
	return StorageSuccess;
}


StorageError FileDataRepository::LoadTeachingResult(
	int handId,
	int locationId,
	int pkgId,
	int dateoffset,
	double& posX,
	double& posY,
	double& posZ)
{
	posX = 0.0;
	posY = 0.0;
	posZ = 0.0;
	return StorageSuccess;
}

StorageError FileDataRepository::LoadHandCamGroup(
	int handId,
	std::vector<int>& camIds)
{
	return StorageSuccess;
}

StorageError FileDataRepository::LoadCamLocationGroup(
    int handId,
    std::vector<int>& locateIds)
{
    return StorageSuccess;
}

StorageError FileDataRepository::LoadLocationIdByName(
	const std::string& locateName,
	int& locationId)
{
	return StorageSuccess;
}

StorageError FileDataRepository::UpdateSequenceRunStatus(int runId, const std::string& status, const std::string& resultSummaryJson)
{
    std::lock_guard<std::mutex> lg(mutex_);
    std::ostringstream ss;
    ss << FileUtils::MakeTimeTag() << " | UPDATE_RUN | id=" << runId << " | status=" << status << " | result=" << resultSummaryJson << "\n";

    std::string logPath = FileUtils::JoinPath(basePath_, "sequence_runs.log");
    std::ofstream ofs(logPath.c_str(), std::ios::app);
    if (!ofs) {
        std::cerr << "[FileDataRepository] Failed to open sequence_runs.log for UpdateSequenceRunStatus\n";
        return StorageWriteFailed;
    }
    ofs << ss.str();
    return StorageSuccess;
}

} // namespace VMF
