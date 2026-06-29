#include "stdafx.h"
#include "Mock/CMockDataRepository.h"

#include <sstream>

namespace VMF
{
    CMockDataRepository::CMockDataRepository()
    {
        m_nextRunId = 1;
    }

    CMockDataRepository::~CMockDataRepository()
    {
    }

    StorageError CMockDataRepository::SaveParam(const std::string& recipe, const std::string& name, const std::string& value)
    {
        std::lock_guard<std::mutex> lg(m_mutex);
        std::string key = recipe + ":" + name;
        m_params[key] = value;
        return StorageSuccess;
    }

    StorageError CMockDataRepository::LoadParam(const std::string& recipe, const std::string& name, std::string& outValue)
    {
        std::lock_guard<std::mutex> lg(m_mutex);
        std::string key = recipe + ":" + name;
        std::map<std::string, std::string>::const_iterator it = m_params.find(key);
        if (it == m_params.end()) return StorageNotFound;
        outValue = it->second;
        return StorageSuccess;
    }

    StorageError CMockDataRepository::SaveImage(const std::string& contextTag, const std::vector<uint8_t>& /*imageData*/, std::string& outPath)
    {
        std::lock_guard<std::mutex> lg(m_mutex);
        std::ostringstream ss;
        ss << "mock://image/" << contextTag;
        outPath = ss.str();
        m_images[contextTag] = outPath;
        return StorageSuccess;
    }

    StorageError CMockDataRepository::SaveSequenceRun(const std::string& sequenceName, const std::string& summary)
    {
        std::lock_guard<std::mutex> lg(m_mutex);
        std::ostringstream key;
        key << "seq:" << sequenceName << ":" << m_nextRunId++;
        m_params[key.str()] = summary;
        return StorageSuccess;
    }

    StorageError CMockDataRepository::Initialize()
    {
        std::lock_guard<std::mutex> lg(m_mutex);
        m_params.clear();
        m_images.clear();
        m_nextRunId = 1;
        return StorageSuccess;
    }

    StorageError CMockDataRepository::Shutdown()
    {
        std::lock_guard<std::mutex> lg(m_mutex);
        return StorageSuccess;
    }

    StorageError CMockDataRepository::CreateSequenceRun(const std::string& sequenceName, const std::string& paramsJson, int& outRunId)
    {
        std::lock_guard<std::mutex> lg(m_mutex);
        outRunId = m_nextRunId++;
        std::ostringstream key;
        key << "run:" << outRunId;
        m_params[key.str()] = sequenceName + "|" + paramsJson;
        return StorageSuccess;
    }

    StorageError CMockDataRepository::SaveZFocusPoint(int runId, double zPosition, double /*score*/, int /*sampleCount*/, const std::string& extraJson)
    {
        std::lock_guard<std::mutex> lg(m_mutex);
        std::ostringstream key;
        key << "zpoint:" << runId << ":" << zPosition;
        m_params[key.str()] = extraJson;
        return StorageSuccess;
    }

    StorageError CMockDataRepository::SaveZFocusResult(int /*camIndex*/, int /*locationId*/, int /*pkgId*/, double /*newFocus*/)
    {
        return StorageSuccess;
    }

    StorageError CMockDataRepository::SavePickerCamDistanceResult(int /*camIndex*/, int /*pkgId*/, double /*narrowX*/, double /*narrowY*/, double /*wideX*/, double /*wideY*/)
    {
        return StorageSuccess;
    }

    StorageError CMockDataRepository::SaveCalibrationPosResult(int /*camIndex*/, int /*locationId*/, int /*pkgId*/, double /*posX*/, double /*posY*/)
    {
        return StorageSuccess;
    }

    StorageError CMockDataRepository::UpdateSequenceRunStatus(int runId, const std::string& status, const std::string& resultSummaryJson)
    {
        std::lock_guard<std::mutex> lg(m_mutex);
        std::ostringstream key;
        key << "runstatus:" << runId;
        m_params[key.str()] = status + "|" + resultSummaryJson;
        return StorageSuccess;
    }

    StorageError CMockDataRepository::SaveHandPitchResult(int /*handId*/, int /*pkgId*/, int /*col*/, int /*row*/, double /*narrowX*/, double /*narrowY*/, double /*wideX*/, double /*wideY*/)
    {
        return StorageSuccess;
    }

    StorageError CMockDataRepository::SaveTeachingResult(int /*handId*/, int /*locationId*/, int /*pkgId*/, double /*posX*/, double /*posY*/, double /*posZ*/)
    {
        return StorageSuccess;
    }

    StorageError CMockDataRepository::LoadInspInitPos(int /*camIndex*/, int /*locationId*/, int /*pkgId*/, double& posX, double& posY, double& focus)
    {
        posX = 0.0;
        posY = 0.0;
        focus = 0.0;
        return StorageNotFound;
    }

    StorageError CMockDataRepository::LoadPickerCamDistance(int /*camIndex*/, int /*pkgId*/, double& narrowX, double& narrowY, double& wideX, double& wideY)
    {
        narrowX = narrowY = wideX = wideY = 0.0;
        return StorageNotFound;
    }

    StorageError CMockDataRepository::LoadHandPitch(int /*handId*/, int /*pkgId*/, int /*row*/, int /*col*/, double& narrowX, double& narrowY, double& wideX, double& wideY)
    {
        narrowX = narrowY = wideX = wideY = 0.0;
        return StorageNotFound;
    }

    StorageError CMockDataRepository::LoadTeachingResult(int /*handId*/, int /*locationId*/, int /*pkgId*/, int /*dateoffset*/, double& posX, double& posY, double& posZ)
    {
        posX = posY = posZ = 0.0;
        return StorageNotFound;
    }

    StorageError CMockDataRepository::LoadHandCamGroup(int /*handId*/, std::vector<int>& camIds)
    {
        camIds.clear();
        return StorageNotFound;
    }

    StorageError CMockDataRepository::LoadCamLocationGroup(int /*camIndex*/, std::vector<int>& locateIds)
    {
        locateIds.clear();
        return StorageNotFound;
    }

    StorageError CMockDataRepository::LoadLocationIdByName(const std::string& locateName, int& locationId)
    {
        (void)locateName;
        locationId = -1;
        return StorageNotFound;
    }

    void CMockDataRepository::ClearAll()
    {
        std::lock_guard<std::mutex> lg(m_mutex);
        m_params.clear();
        m_images.clear();
        m_nextRunId = 1;
    }

} // namespace VMF

