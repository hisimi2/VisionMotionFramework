#include "stdafx.h"
#include "FileDataRepository.h"
#include "FileUtils.h"
#include "CompatUtils.h"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <iostream>
#include <mutex>
#include <algorithm>

namespace VMF
{
    FileDataRepository::FileDataRepository(const std::string& basePath)
        : basePath_(basePath) {
    }

    FileDataRepository::~FileDataRepository() = default;

    StorageError FileDataRepository::SaveParam(const std::string& recipe, const std::string& name, const std::string& value) {
        std::lock_guard<std::mutex> lg(mutex_);
        std::ofstream ofs(FileUtils::MakeParamPath(basePath_, recipe).c_str(), std::ios::app);
        if (!ofs) {
            std::cerr << "[FileDataRepository] Failed to open file for writing: " << FileUtils::MakeParamPath(basePath_, recipe) << std::endl;
            return StorageWriteFailed;
        }

        ofs << name << "=" << value << "\n";
        return StorageSuccess;
    }

    StorageError FileDataRepository::LoadParam(const std::string& recipe, const std::string& name, std::string& outValue)
    {
        std::lock_guard<std::mutex> lg(mutex_);
        std::ifstream ifs(FileUtils::MakeParamPath(basePath_, recipe).c_str());

        if (!ifs)
        {
            std::cerr << "[FileDataRepository] Failed to open file for reading: " << FileUtils::MakeParamPath(basePath_, recipe) << std::endl;
            return StorageFileNotFound;
        }

        std::string line;

        while (std::getline(ifs, line))
        {
            if (line.empty()) continue;

            size_t pos = line.find('=');
            if (pos == std::string::npos || name.empty()) continue;

            if (pos == name.length() && line.substr(0, pos) == name)
            {
                outValue = line.substr(pos + 1);
                return StorageSuccess;
            }
        }
        return StorageFileNotFound;
    }

    StorageError FileDataRepository::SaveImage(const std::string& contextTag, const std::vector<uint8_t>& imageData, std::string& outPath)
    {
        std::lock_guard<std::mutex> lg(mutex_);
        outPath = FileUtils::MakeImagePath(basePath_, contextTag);
        std::ofstream ofs(outPath.c_str(), std::ios::binary);
        if (!ofs) {
            std::cerr << "[FileDataRepository] Failed to open file for writing: " << outPath << std::endl;
            return StorageWriteFailed;
        }

        if (!imageData.empty())
        {
            ofs.write(reinterpret_cast<const char*>(&imageData[0]), static_cast<std::streamsize>(imageData.size()));
        }
        return StorageSuccess;
    }

    StorageError FileDataRepository::Initialize() {
        return StorageSuccess;
    }

    StorageError FileDataRepository::Shutdown() {
        return StorageSuccess;
    }

    // ─────────────────────────────────────────────
    // 검사 실행 이력 관리 (파일 기반 간단 구현)
    // ─────────────────────────────────────────────

    StorageError FileDataRepository::CreateInspectionRun(InspectionType type, const std::string& runId,
        const std::string& paramsJson, int& outDbRunId)
    {
        std::lock_guard<std::mutex> lg(mutex_);
        std::time_t t = std::time(nullptr);
        outDbRunId = static_cast<int>(t & 0x7FFFFFFF);

        std::string typeStr;
        switch (type) {
            case InspectionType::PLVI: typeStr = "PLVI"; break;
            case InspectionType::Orientation: typeStr = "Orientation"; break;
            case InspectionType::COK_ID: typeStr = "COK_ID"; break;
            case InspectionType::VAT: typeStr = "VAT"; break;
            case InspectionType::Piggyback: typeStr = "Piggyback"; break;
            case InspectionType::BallInspection: typeStr = "BallInspection"; break;
            case InspectionType::SetCok: typeStr = "SetCok"; break;
            default: typeStr = "Unknown"; break;
        }

        std::ostringstream ss;
        ss << FileUtils::MakeTimeTag() << " | CREATE_INSP_RUN | id=" << outDbRunId
           << " | type=" << typeStr << " | runId=" << runId << " | params=" << paramsJson << "\n";

        std::string logPath = FileUtils::JoinPath(basePath_, "inspection_runs.log");
        std::ofstream ofs(logPath.c_str(), std::ios::app);
        if (!ofs) {
            std::cerr << "[FileDataRepository] Failed to open inspection_runs.log for CreateInspectionRun\n";
            return StorageWriteFailed;
        }
        ofs << ss.str();
        return StorageSuccess;
    }

    StorageError FileDataRepository::UpdateInspectionRunStatus(int dbRunId, const std::string& status,
        const std::string& resultJson, int errorCode)
    {
        std::lock_guard<std::mutex> lg(mutex_);
        std::ostringstream ss;
        ss << FileUtils::MakeTimeTag() << " | UPDATE_INSP_RUN | id=" << dbRunId
           << " | status=" << status << " | result=" << resultJson;
        if (errorCode >= 0) {
            ss << " | errorCode=" << errorCode;
        }
        ss << "\n";

        std::string logPath = FileUtils::JoinPath(basePath_, "inspection_runs.log");
        std::ofstream ofs(logPath.c_str(), std::ios::app);

        if (!ofs)
        {
            std::cerr << "[FileDataRepository] Failed to open inspection_runs.log for UpdateInspectionRunStatus\n";
            return StorageWriteFailed;
        }
        ofs << ss.str();
        return StorageSuccess;
    }

    StorageError FileDataRepository::LoadInspectionRun(int dbRunId, std::string& outTypeCode, std::string& outRunId,
        std::string& outStatus, std::string& outResultJson, int& outErrorCode)
    {
        (void)dbRunId;
        outTypeCode.clear();
        outRunId.clear();
        outStatus.clear();
        outResultJson.clear();
        outErrorCode = -1;
        return StorageNotFound;
    }

    // ─────────────────────────────────────────────
    // 검사 결과 상세 저장/로드 (파일 기반 간단 구현)
    // ─────────────────────────────────────────────

    StorageError FileDataRepository::SaveInspectionResult(int dbRunId, const std::string& resultType,
        int resultIndex, const std::string& resultJson)
    {
        std::lock_guard<std::mutex> lg(mutex_);
        std::ostringstream ss;
        ss << FileUtils::MakeTimeTag() << " | INSP_RESULT | runId=" << dbRunId
           << " | type=" << resultType << " | index=" << resultIndex << " | data=" << resultJson << "\n";

        std::string logPath = FileUtils::JoinPath(basePath_, "inspection_results.log");
        std::ofstream ofs(logPath.c_str(), std::ios::app);
        if (!ofs) {
            std::cerr << "[FileDataRepository] Failed to open inspection_results.log for SaveInspectionResult\n";
            return StorageWriteFailed;
        }
        ofs << ss.str();
        return StorageSuccess;
    }

    StorageError FileDataRepository::LoadInspectionResults(int dbRunId, std::vector<InspectionResultItem>& outResults)
    {
        (void)dbRunId;
        outResults.clear();
        return StorageNotFound;
    }

    // ─────────────────────────────────────────────
    // 시퀀스 실행 이력
    // ─────────────────────────────────────────────

    StorageError FileDataRepository::CreateSequenceRun(const std::string& sequenceName, const std::string& paramsJson, int& outRunId)
    {
        std::lock_guard<std::mutex> lg(mutex_);
        std::time_t t = std::time(nullptr);
        outRunId = static_cast<int>(t & 0x7FFFFFFF);

        std::ostringstream ss;
        ss << FileUtils::MakeTimeTag() << " | CREATE_RUN | id=" << outRunId << " | type=" << sequenceName << " | params=" << paramsJson << "\n";

        std::string logPath = FileUtils::JoinPath(basePath_, "sequence_runs.log");
        std::ofstream ofs(logPath.c_str(), std::ios::app);
        if (!ofs) {
            std::cerr << "[FileDataRepository] Failed to open sequence_runs.log for CreateSequenceRun\n";
            return StorageWriteFailed;
        }
        ofs << ss.str();
        return StorageSuccess;
    }

    StorageError FileDataRepository::UpdateSequenceRunStatus(int runId, const std::string& status,
        const std::string& resultSummaryJson)
    {
        std::lock_guard<std::mutex> lg(mutex_);
        std::ostringstream ss;
        ss << FileUtils::MakeTimeTag() << " | UPDATE_RUN | id=" << runId << " | status=" << status << " | result=" << resultSummaryJson << "\n";

        std::string logPath = FileUtils::JoinPath(basePath_, "sequence_runs.log");
        std::ofstream ofs(logPath.c_str(), std::ios::app);

        if (!ofs)
        {
            std::cerr << "[FileDataRepository] Failed to open sequence_runs.log for UpdateSequenceRunStatus\n";
            return StorageWriteFailed;
        }
        ofs << ss.str();
        return StorageSuccess;
    }

    // ─────────────────────────────────────────────
    // Z-포커스 관련
    // ─────────────────────────────────────────────

    StorageError FileDataRepository::SaveZFocusPoint(int runId, double zPosition, double score,
        int sampleCount, const std::string& extraJson)
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

    StorageError FileDataRepository::LoadInspInitPos(
        int camIndex, int locationId, int pkgId,
        double& outPosX, double& outPosY, double& outFocus)
    {
        (void)camIndex; (void)locationId; (void)pkgId;
        outPosX = 0.0;
        outPosY = 0.0;
        outFocus = 0.0;
        return StorageNotFound;
    }

    // ─────────────────────────────────────────────
    // 피커-캠 거리 관련
    // ─────────────────────────────────────────────

    StorageError FileDataRepository::SavePickerCamDistanceResult(int camIndex, int pkgId,
        double narrowX, double narrowY, double wideX, double wideY)
    {
        (void)camIndex; (void)pkgId; (void)narrowX; (void)narrowY; (void)wideX; (void)wideY;
        return StorageSuccess;
    }

    StorageError FileDataRepository::LoadPickerCamDistance(int camIndex, int pkgId,
        double& outNarrowX, double& outNarrowY, double& outWideX, double& outWideY)
    {
        (void)camIndex; (void)pkgId;
        outNarrowX = 0.0;
        outNarrowY = 0.0;
        outWideX = 0.0;
        outWideY = 0.0;
        return StorageNotFound;
    }

    // ─────────────────────────────────────────────
    // 캘리브레이션 위치 관련
    // ─────────────────────────────────────────────

    StorageError FileDataRepository::SaveCalibrationPosResult(int camIndex, int locationId, int pkgId,
        double posX, double posY)
    {
        (void)camIndex; (void)locationId; (void)pkgId; (void)posX; (void)posY;
        return StorageSuccess;
    }

    // ─────────────────────────────────────────────
    // 핸드 피치 관련
    // ─────────────────────────────────────────────

    StorageError FileDataRepository::SaveHandPitchResult(int handId, int pkgId, int row, int col,
        double narrowX, double narrowY, double wideX, double wideY)
    {
        (void)handId; (void)pkgId; (void)row; (void)col; (void)narrowX; (void)narrowY; (void)wideX; (void)wideY;
        return StorageSuccess;
    }

    StorageError FileDataRepository::LoadHandPitch(int handId, int pkgId, int row, int col,
        double& outNarrowX, double& outNarrowY, double& outWideX, double& outWideY)
    {
        (void)handId; (void)pkgId; (void)row; (void)col;
        outNarrowX = 0.0;
        outNarrowY = 0.0;
        outWideX = 0.0;
        outWideY = 0.0;
        return StorageNotFound;
    }

    // ─────────────────────────────────────────────
    // 티칭 결과 관련
    // ─────────────────────────────────────────────

    StorageError FileDataRepository::SaveTeachingResult(int handId, int locationId, int pkgId,
        double posX, double posY, double posZ)
    {
        (void)handId; (void)locationId; (void)pkgId; (void)posX; (void)posY; (void)posZ;
        return StorageSuccess;
    }

    StorageError FileDataRepository::LoadTeachingResult(int handId, int locationId, int pkgId, int dateoffset,
        double& outPosX, double& outPosY, double& outPosZ)
    {
        (void)handId; (void)locationId; (void)pkgId; (void)dateoffset;
        outPosX = 0.0;
        outPosY = 0.0;
        outPosZ = 0.0;
        return StorageNotFound;
    }

    // ─────────────────────────────────────────────
    // 그룹 매핑 관련
    // ─────────────────────────────────────────────

    StorageError FileDataRepository::LoadHandCamGroup(int handId, std::vector<int>& outCamIds)
    {
        (void)handId;
        outCamIds.clear();
        return StorageNotFound;
    }

    StorageError FileDataRepository::LoadCamLocationGroup(int camIndex, std::vector<int>& outLocateIds)
    {
        (void)camIndex;
        outLocateIds.clear();
        return StorageNotFound;
    }

    StorageError FileDataRepository::LoadLocationIdByName(const std::string& locateName, int& outLocationId)
    {
        (void)locateName;
        outLocationId = -1;
        return StorageNotFound;
    }

    // ─────────────────────────────────────────────
    // 검사 위치 마스터 (신규)
    // ─────────────────────────────────────────────

    StorageError FileDataRepository::LoadInspectionLocation(int locationCode, std::string& outLocationName,
        std::string& outLocationType)
    {
        (void)locationCode;
        outLocationName.clear();
        outLocationType.clear();
        return StorageNotFound;
    }

StorageError FileDataRepository::LoadAllInspectionLocations(std::vector<InspectionLocationInfo>& outLocations)
    {
        outLocations.clear();
        return StorageNotFound;
    }

    // --- Socket 마스터 (신규) ---

    StorageError FileDataRepository::LoadSocketMaster(int socketNo, int& outSocketType, int& outStationNo)
    {
        (void)socketNo;
        outSocketType = 0;
        outStationNo = 0;
        return StorageNotFound;
    }

    StorageError FileDataRepository::LoadAllSocketMasters(std::vector<SocketMasterInfo>& outSockets)
    {
        outSockets.clear();
        return StorageNotFound;
    }
} // namespace VMF

