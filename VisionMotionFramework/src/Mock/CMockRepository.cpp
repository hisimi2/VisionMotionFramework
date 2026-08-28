#include "stdafx.h"
#include "Mock/CMockRepository.h"

#include <sstream>

namespace VMF
{
    CMockRepository::CMockRepository()
    {
        m_nextRunId = 1;
    }

    CMockRepository::~CMockRepository()
    {
    }

    StorageError CMockRepository::SaveParam(const std::string& recipe, const std::string& name, const std::string& value)
    {
        std::lock_guard<std::mutex> lg(m_mutex);
        std::string key = recipe + ":" + name;
        m_params[key] = value;
        return StorageSuccess;
    }

    StorageError CMockRepository::LoadParam(const std::string& recipe, const std::string& name, std::string& outValue)
    {
        std::lock_guard<std::mutex> lg(m_mutex);
        std::string key = recipe + ":" + name;
        std::map<std::string, std::string>::const_iterator it = m_params.find(key);
        if (it == m_params.end()) return StorageNotFound;
        outValue = it->second;
        return StorageSuccess;
    }

    StorageError CMockRepository::SaveImage(const std::string& contextTag, const std::vector<uint8_t>& /*imageData*/, std::string& outPath)
    {
        std::lock_guard<std::mutex> lg(m_mutex);
        std::ostringstream ss;
        ss << "mock://image/" << contextTag;
        outPath = ss.str();
        m_images[contextTag] = outPath;
        return StorageSuccess;
    }

    StorageError CMockRepository::SaveSequenceRun(const std::string& sequenceName, const std::string& summary)
    {
        std::lock_guard<std::mutex> lg(m_mutex);
        std::ostringstream key;
        key << "seq:" << sequenceName << ":" << m_nextRunId++;
        m_params[key.str()] = summary;
        return StorageSuccess;
    }

    StorageError CMockRepository::Initialize()
    {
        std::lock_guard<std::mutex> lg(m_mutex);
        m_params.clear();
        m_images.clear();
        m_nextRunId = 1;
        return StorageSuccess;
    }

    StorageError CMockRepository::Shutdown()
    {
        std::lock_guard<std::mutex> lg(m_mutex);
        return StorageSuccess;
    }

    StorageError CMockRepository::CreateSequenceRun(const std::string& sequenceName, const std::string& paramsJson, int& outRunId)
    {
        std::lock_guard<std::mutex> lg(m_mutex);
        outRunId = m_nextRunId++;
        std::ostringstream key;
        key << "run:" << outRunId;
        m_params[key.str()] = sequenceName + "|" + paramsJson;
        return StorageSuccess;
    }

    StorageError CMockRepository::SaveZFocusPoint(int runId, double zPosition, double /*score*/, int /*sampleCount*/, const std::string& extraJson)
    {
        std::lock_guard<std::mutex> lg(m_mutex);
        std::ostringstream key;
        key << "zpoint:" << runId << ":" << zPosition;
        m_params[key.str()] = extraJson;
        return StorageSuccess;
    }

    StorageError CMockRepository::SaveZFocusResult(int /*camIndex*/, int /*locationId*/, int /*pkgId*/, double /*newFocus*/)
    {
        return StorageSuccess;
    }

    StorageError CMockRepository::SavePickerCamDistanceResult(int /*camIndex*/, int /*pkgId*/, double /*narrowX*/, double /*narrowY*/, double /*wideX*/, double /*wideY*/)
    {
        return StorageSuccess;
    }

    StorageError CMockRepository::SaveCalibrationPosResult(int /*camIndex*/, int /*locationId*/, int /*pkgId*/, double /*posX*/, double /*posY*/)
    {
        return StorageSuccess;
    }

    StorageError CMockRepository::UpdateSequenceRunStatus(int runId, const std::string& status, const std::string& resultSummaryJson)
    {
        std::lock_guard<std::mutex> lg(m_mutex);
        std::ostringstream key;
        key << "runstatus:" << runId;
        m_params[key.str()] = status + "|" + resultSummaryJson;
        return StorageSuccess;
    }

    StorageError CMockRepository::SaveHandPitchResult(int /*handId*/, int /*pkgId*/, int /*col*/, int /*row*/, double /*narrowX*/, double /*narrowY*/, double /*wideX*/, double /*wideY*/)
    {
        return StorageSuccess;
    }

    StorageError CMockRepository::SaveTeachingResult(int /*handId*/, int /*locationId*/, int /*pkgId*/, double /*posX*/, double /*posY*/, double /*posZ*/)
    {
        return StorageSuccess;
    }

    StorageError CMockRepository::LoadInspInitPos(int /*camIndex*/, int /*locationId*/, int /*pkgId*/, double& posX, double& posY, double& focus)
    {
        posX = 0.0;
        posY = 0.0;
        focus = 0.0;
        return StorageNotFound;
    }

    StorageError CMockRepository::LoadPickerCamDistance(int /*camIndex*/, int /*pkgId*/, double& narrowX, double& narrowY, double& wideX, double& wideY)
    {
        narrowX = narrowY = wideX = wideY = 0.0;
        return StorageNotFound;
    }

    StorageError CMockRepository::LoadHandPitch(int /*handId*/, int /*pkgId*/, int /*row*/, int /*col*/, double& narrowX, double& narrowY, double& wideX, double& wideY)
    {
        narrowX = narrowY = wideX = wideY = 0.0;
        return StorageNotFound;
    }

    StorageError CMockRepository::LoadTeachingResult(int /*handId*/, int /*locationId*/, int /*pkgId*/, int /*dateoffset*/, double& posX, double& posY, double& posZ)
    {
        posX = posY = posZ = 0.0;
        return StorageNotFound;
    }

    StorageError CMockRepository::LoadHandCamGroup(int /*handId*/, std::vector<int>& camIds)
    {
        camIds.clear();
        return StorageNotFound;
    }

    StorageError CMockRepository::LoadCamLocationGroup(int /*camIndex*/, std::vector<int>& locateIds)
    {
        locateIds.clear();
        return StorageNotFound;
    }

    StorageError CMockRepository::LoadLocationIdByName(const std::string& locateName, int& locationId)
    {
        (void)locateName;
        locationId = -1;
        return StorageNotFound;
    }

    void CMockRepository::ClearAll()
    {
        std::lock_guard<std::mutex> lg(m_mutex);
        m_params.clear();
        m_images.clear();
        m_nextRunId = 1;
    }

} // namespace VMF

