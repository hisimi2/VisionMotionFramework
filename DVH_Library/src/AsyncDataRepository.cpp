// [필수] 미리 컴파일된 헤더
#include "stdafx.h"

// Boost 헤더
#include <boost/thread.hpp>
#include <boost/atomic.hpp>
#include <boost/date_time/posix_time/posix_time.hpp> 

#include "AsyncDataRepository.h"

namespace DVH_VAT {

// Impl 구조체 정의
struct AsyncDataRepository::Impl {
    struct PendingZPoint {
        int runId;
        double zPosition;
        double score;
        int sampleCount;
        std::string extraJson;
    };
    
    struct PendingResult {
        int camIndex;
        int locationId;
        int pkgId;
        double newFocus;
    };

    struct PendingStatus {
        int runId;
        std::string status;
        std::string resultSummaryJson;
    };

    IDataRepository* m_inner;
    bool m_ownInner;

    std::deque<PendingZPoint> m_pointQueue;
    std::deque<PendingResult> m_resultQueue;
    std::deque<PendingStatus> m_statusQueue;

    boost::mutex m_mutex;
    boost::condition_variable m_cv;
    boost::thread m_worker;
    boost::atomic<bool> m_stopRequested;
    
    int m_maxRetries;

    Impl(IDataRepository* inner, bool ownInner) 
        : m_inner(inner), m_ownInner(ownInner), m_stopRequested(false), m_maxRetries(3) {}

    void WorkerLoop()
    {
        while (true)
        {
            std::deque<PendingZPoint> localPoints;
            std::deque<PendingResult> localResults;
            std::deque<PendingStatus> localStatuses;

            {
                UniqueLockType lk(m_mutex);
                while (m_pointQueue.empty() && m_resultQueue.empty() && m_statusQueue.empty() && !m_stopRequested) {
                    m_cv.wait(lk);
                }
                
                if (m_stopRequested && m_pointQueue.empty() && m_resultQueue.empty() && m_statusQueue.empty()) {
                    break;
                }
                
                localPoints.swap(m_pointQueue);
                localResults.swap(m_resultQueue);
                localStatuses.swap(m_statusQueue);
            }

            // [1] ZFocusPoint 처리
            for (size_t i = 0; i < localPoints.size(); ++i) {
                const PendingZPoint &item = localPoints[i];
                if (!m_inner) continue;
                
                StorageError res = StorageGeneral;
                for (int attempt = 0; attempt < m_maxRetries; ++attempt) {
                    res = m_inner->SaveZFocusPoint(item.runId, item.zPosition, item.score, item.sampleCount, item.extraJson);
                    if (res == StorageSuccess) break;
                    try { boost::this_thread::sleep(boost::posix_time::milliseconds(100)); } catch (...) {}
                }
            }

            // [2] ZFocusResult 처리
            for (size_t i = 0; i < localResults.size(); ++i) {
                const PendingResult &item = localResults[i];
                if (!m_inner) continue;
                
                StorageError res = StorageGeneral;
                for (int attempt = 0; attempt < m_maxRetries; ++attempt) {
                    res = m_inner->SaveZFocusResult(item.camIndex, item.locationId, item.pkgId, item.newFocus);
                    if (res == StorageSuccess) break;
                    try { boost::this_thread::sleep(boost::posix_time::milliseconds(100)); } catch (...) {}
                }
            }

            // [3] Status 처리
            for (size_t i = 0; i < localStatuses.size(); ++i) {
                const PendingStatus &item = localStatuses[i];
                if (!m_inner) continue;
                
                StorageError res = StorageGeneral;
                for (int attempt = 0; attempt < m_maxRetries; ++attempt) {
                    res = m_inner->UpdateSequenceRunStatus(item.runId, item.status, item.resultSummaryJson);
                    if (res == StorageSuccess) break;
                    try { boost::this_thread::sleep(boost::posix_time::milliseconds(100)); } catch (...) {}
                }
            }
        } // while
    }
};

AsyncDataRepository::AsyncDataRepository(IDataRepository* inner, bool ownInner)
    : m_pImpl(new Impl(inner, ownInner))
{
    m_pImpl->m_worker = boost::thread(&AsyncDataRepository::Impl::WorkerLoop, m_pImpl);
}

AsyncDataRepository::~AsyncDataRepository()
{
    if (m_pImpl)
    {
        if (m_pImpl->m_worker.joinable())
        {
            m_pImpl->m_stopRequested = true;
            m_pImpl->m_cv.notify_one();
            m_pImpl->m_worker.join();
        }
        delete m_pImpl;
        m_pImpl = NULL;
    }
}

StorageError AsyncDataRepository::SaveZFocusPoint(int runId, double zPosition, double score, int sampleCount, const std::string& extraJson)
{
    if(!m_pImpl) return StorageErrorWriteFailed;

    UniqueLockType lock(m_pImpl->m_mutex);
    Impl::PendingZPoint pt;
    pt.runId = runId;
    pt.zPosition = zPosition;
    pt.score = score;
    pt.sampleCount = sampleCount;
    pt.extraJson = extraJson;
    
    m_pImpl->m_pointQueue.push_back(pt);
    m_pImpl->m_cv.notify_one();
    
    return StorageSuccess;
}

StorageError AsyncDataRepository::SaveZFocusResult(int camIndex, int locationId, int pkgId, double newFocus) 
{
    if(!m_pImpl) return StorageErrorWriteFailed;

    UniqueLockType lock(m_pImpl->m_mutex);
    Impl::PendingResult res;
    res.camIndex = camIndex;
    res.locationId = locationId;
    res.pkgId = pkgId;
    res.newFocus = newFocus;

    m_pImpl->m_resultQueue.push_back(res);
    m_pImpl->m_cv.notify_one();

    return StorageSuccess;
}

StorageError AsyncDataRepository::UpdateSequenceRunStatus(int runId, const std::string& status, const std::string& resultSummaryJson) 
{
    if(!m_pImpl) return StorageErrorWriteFailed;

    UniqueLockType lock(m_pImpl->m_mutex);
    Impl::PendingStatus st;
    // [수정] pt -> st 오타 수정
    st.runId = runId;
    st.status = status;
    st.resultSummaryJson = resultSummaryJson;

    m_pImpl->m_statusQueue.push_back(st);
    m_pImpl->m_cv.notify_one();

    return StorageSuccess;
}

// 링킹 에러 방지를 위한 위임 구현
StorageError AsyncDataRepository::SaveParam(const std::string& r, const std::string& n, const std::string& v) { return m_pImpl->m_inner->SaveParam(r, n, v); }
StorageError AsyncDataRepository::LoadParam(const std::string& r, const std::string& n, std::string& v) { return m_pImpl->m_inner->LoadParam(r, n, v); }
StorageError AsyncDataRepository::SaveImage(const std::string& t, const std::vector<uint8_t>& d, std::string& p) { return m_pImpl->m_inner->SaveImage(t, d, p); }
StorageError AsyncDataRepository::SaveSequenceRun(const std::string& n, const std::string& s) { return m_pImpl->m_inner->SaveSequenceRun(n, s); }
StorageError AsyncDataRepository::Initialize() { return m_pImpl->m_inner->Initialize(); }
StorageError AsyncDataRepository::Shutdown() { return m_pImpl->m_inner->Shutdown(); }
StorageError AsyncDataRepository::CreateSequenceRun(const std::string& sequenceName, const std::string& p, int& id) { return m_pImpl->m_inner->CreateSequenceRun(sequenceName, p, id); }
StorageError AsyncDataRepository::SavePickerCamDistanceResult(int c, int p, double nX, double nY, double wX, double wY) { return m_pImpl->m_inner->SavePickerCamDistanceResult(c, p, nX, nY, wX, wY); }
StorageError AsyncDataRepository::SaveCalibrationPosResult(int c, int l, int p, double x, double y) { return m_pImpl->m_inner->SaveCalibrationPosResult(c, l, p, x, y); }
StorageError AsyncDataRepository::SaveHandPitchResult(int h, int p, int c, int r, double nX, double nY, double wX, double wY) { return m_pImpl->m_inner->SaveHandPitchResult(h, p, c, r, nX, nY, wX, wY); }
StorageError AsyncDataRepository::SaveTeachingResult(int h, int l, int p, double x, double y, double z) { return m_pImpl->m_inner->SaveTeachingResult(h, l, p, x, y, z); }
StorageError AsyncDataRepository::LoadInspInitPos(int c, int l, int p, double& x, double& y, double& f) { return m_pImpl->m_inner->LoadInspInitPos(c, l, p, x, y, f); }
StorageError AsyncDataRepository::LoadPickerCamDistance(int c, int p, double& nR, double& nL, double& wR, double& wL) { return m_pImpl->m_inner->LoadPickerCamDistance(c, p, nR, nL, wR, wL); }
StorageError AsyncDataRepository::LoadHandPitch(int h, int p, int r, int c, double& nX, double& nY, double& wX, double& wY) { return m_pImpl->m_inner->LoadHandPitch(h, p, r, c, nX, nY, wX, wY); }
StorageError AsyncDataRepository::LoadTeachingResult(int h, int l, int p, int d, double& x, double& y, double& z) { return m_pImpl->m_inner->LoadTeachingResult(h, l, p, d, x, y, z); }
StorageError AsyncDataRepository::LoadHandCamGroup(int h, std::vector<int>& c) { return m_pImpl->m_inner->LoadHandCamGroup(h, c); }
StorageError AsyncDataRepository::LoadCamLocationGroup(int handId, std::vector<int>& locateIds){ return StorageSuccess; }
StorageError AsyncDataRepository::LoadLocationIdByName(const std::string& n, int& id) { return m_pImpl->m_inner->LoadLocationIdByName(n, id); }

} // namespace DVH_VAT
