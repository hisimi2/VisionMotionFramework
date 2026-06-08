#include "stdafx.h"

#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <deque>

#include "AsyncDataRepository.h"

namespace VMF {

void AsyncDataRepository::WorkerLoop()
{
    while (true)
    {
        std::deque<PendingZPoint> localPoints;
        std::deque<PendingResult> localResults;
        std::deque<PendingStatus> localStatuses;

        {
            std::unique_lock<std::mutex> lk(m_mutex);
            
            m_cv.wait(lk, [this]() { 
                return m_stopRequested || !m_pointQueue.empty() || !m_resultQueue.empty() || !m_statusQueue.empty(); 
            });
            
            if (m_stopRequested && m_pointQueue.empty() && m_resultQueue.empty() && m_statusQueue.empty()) {
                break;
            }
            
            localPoints.swap(m_pointQueue);
            localResults.swap(m_resultQueue);
            localStatuses.swap(m_statusQueue);
        }

        // [1] ZFocusPoint 처리
        for (const auto& item : localPoints) {
            if (!m_inner) continue;
            
            StorageError res = StorageGeneral;
            for (int attempt = 0; attempt < m_maxRetries; ++attempt) {
                res = m_inner->SaveZFocusPoint(item.runId, item.zPosition, item.score, item.sampleCount, item.extraJson);
                if (res == StorageSuccess) break;
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
        }

        // [2] ZFocusResult 처리
        for (const auto& item : localResults) {
            if (!m_inner) continue;
            
            StorageError res = StorageGeneral;
            for (int attempt = 0; attempt < m_maxRetries; ++attempt) {
                res = m_inner->SaveZFocusResult(item.camIndex, item.locationId, item.pkgId, item.newFocus);
                if (res == StorageSuccess) break;
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
        }

        // [3] Status 처리
        for (const auto& item : localStatuses) {
            if (!m_inner) continue;
            
            StorageError res = StorageGeneral;
            for (int attempt = 0; attempt < m_maxRetries; ++attempt) {
                res = m_inner->UpdateSequenceRunStatus(item.runId, item.status, item.resultSummaryJson);
                if (res == StorageSuccess) break;
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
        }
    } // while
}

AsyncDataRepository::AsyncDataRepository(IDataRepository* inner, bool ownInner)
    : m_inner(inner)
    , m_ownInner(ownInner)
    , m_stopRequested(false)
    , m_maxRetries(3)
{
    m_worker = std::thread(&AsyncDataRepository::WorkerLoop, this);
}

AsyncDataRepository::~AsyncDataRepository()
{
    if (m_worker.joinable())
    {
        m_stopRequested = true;
        m_cv.notify_one();
        m_worker.join();
    }

    if (m_ownInner && m_inner)
    {
        delete m_inner;
        m_inner = nullptr;
    }
}

StorageError AsyncDataRepository::SaveZFocusPoint(int runId, double zPosition, double score, int sampleCount, const std::string& extraJson)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    PendingZPoint pt;
    pt.runId = runId;
    pt.zPosition = zPosition;
    pt.score = score;
    pt.sampleCount = sampleCount;
    pt.extraJson = extraJson;
    
    m_pointQueue.push_back(std::move(pt));
    m_cv.notify_one();
    
    return StorageSuccess;
}

StorageError AsyncDataRepository::SaveZFocusResult(int camIndex, int locationId, int pkgId, double newFocus) 
{
    std::lock_guard<std::mutex> lock(m_mutex);
    PendingResult res;
    res.camIndex = camIndex;
    res.locationId = locationId;
    res.pkgId = pkgId;
    res.newFocus = newFocus;

    m_resultQueue.push_back(std::move(res));
    m_cv.notify_one();

    return StorageSuccess;
}

StorageError AsyncDataRepository::UpdateSequenceRunStatus(int runId, const std::string& status, const std::string& resultSummaryJson) 
{
    std::lock_guard<std::mutex> lock(m_mutex);
    PendingStatus st;
    st.runId = runId;
    st.status = status;
    st.resultSummaryJson = resultSummaryJson;

    m_statusQueue.push_back(std::move(st));
    m_cv.notify_one();

    return StorageSuccess;
}

// 링킹 에러 방지를 위한 위임 구현
StorageError AsyncDataRepository::SaveParam(const std::string& r, const std::string& n, const std::string& v) { return m_inner->SaveParam(r, n, v); }
StorageError AsyncDataRepository::LoadParam(const std::string& r, const std::string& n, std::string& v) { return m_inner->LoadParam(r, n, v); }
StorageError AsyncDataRepository::SaveImage(const std::string& t, const std::vector<uint8_t>& d, std::string& p) { return m_inner->SaveImage(t, d, p); }
StorageError AsyncDataRepository::SaveSequenceRun(const std::string& n, const std::string& s) { return m_inner->SaveSequenceRun(n, s); }
StorageError AsyncDataRepository::Initialize() { return m_inner->Initialize(); }
StorageError AsyncDataRepository::Shutdown() { return m_inner->Shutdown(); }
StorageError AsyncDataRepository::CreateSequenceRun(const std::string& sequenceName, const std::string& p, int& id) { return m_inner->CreateSequenceRun(sequenceName, p, id); }
StorageError AsyncDataRepository::SavePickerCamDistanceResult(int c, int p, double nX, double nY, double wX, double wY) { return m_inner->SavePickerCamDistanceResult(c, p, nX, nY, wX, wY); }
StorageError AsyncDataRepository::SaveCalibrationPosResult(int c, int l, int p, double x, double y) { return m_inner->SaveCalibrationPosResult(c, l, p, x, y); }
StorageError AsyncDataRepository::SaveHandPitchResult(int h, int p, int c, int r, double nX, double nY, double wX, double wY) { return m_inner->SaveHandPitchResult(h, p, c, r, nX, nY, wX, wY); }
StorageError AsyncDataRepository::SaveTeachingResult(int h, int l, int p, double x, double y, double z) { return m_inner->SaveTeachingResult(h, l, p, x, y, z); }
StorageError AsyncDataRepository::LoadInspInitPos(int c, int l, int p, double& x, double& y, double& f) { return m_inner->LoadInspInitPos(c, l, p, x, y, f); }
StorageError AsyncDataRepository::LoadPickerCamDistance(int c, int p, double& nR, double& nL, double& wR, double& wL) { return m_inner->LoadPickerCamDistance(c, p, nR, nL, wR, wL); }
StorageError AsyncDataRepository::LoadHandPitch(int h, int p, int r, int c, double& nX, double& nY, double& wX, double& wY) { return m_inner->LoadHandPitch(h, p, r, c, nX, nY, wX, wY); }
StorageError AsyncDataRepository::LoadTeachingResult(int h, int l, int p, int d, double& x, double& y, double& z) { return m_inner->LoadTeachingResult(h, l, p, d, x, y, z); }
StorageError AsyncDataRepository::LoadHandCamGroup(int h, std::vector<int>& c) { return m_inner->LoadHandCamGroup(h, c); }
StorageError AsyncDataRepository::LoadCamLocationGroup(int camIndex, std::vector<int>& locateIds){ return m_inner->LoadCamLocationGroup(camIndex, locateIds); }
StorageError AsyncDataRepository::LoadLocationIdByName(const std::string& n, int& id) { return m_inner->LoadLocationIdByName(n, id); }

} // namespace VMF
