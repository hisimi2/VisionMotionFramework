// [필수] 미리 컴파일된 헤더
#include "stdafx.h"

// Boost 대신 C++ 표준 라이브러리 사용
#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <deque>

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

    // 표준 라이브러리로 교체
    std::mutex m_mutex;
    std::condition_variable m_cv;
    std::thread m_worker;
    std::atomic<bool> m_stopRequested;
    
    int m_maxRetries;

    Impl(IDataRepository* inner, bool ownInner) 
        : m_inner(inner), m_ownInner(ownInner), m_stopRequested(false), m_maxRetries(3) {}

    ~Impl() {
        if (m_ownInner && m_inner) {
            delete m_inner;
            m_inner = nullptr;
        }
    }

    void WorkerLoop()
    {
        while (true)
        {
            std::deque<PendingZPoint> localPoints;
            std::deque<PendingResult> localResults;
            std::deque<PendingStatus> localStatuses;

            {
                std::unique_lock<std::mutex> lk(m_mutex);
                
                // C++11: 람다를 사용해 spurious wakeup 방지 및 가독성 개선
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

            // [1] ZFocusPoint 처리 (C++11 range-based for 사용)
            for (const auto& item : localPoints) {
                if (!m_inner) continue;
                
                StorageError res = StorageGeneral;
                for (int attempt = 0; attempt < m_maxRetries; ++attempt) {
                    res = m_inner->SaveZFocusPoint(item.runId, item.zPosition, item.score, item.sampleCount, item.extraJson);
                    if (res == StorageSuccess) break;
                    std::this_thread::sleep_for(std::chrono::milliseconds(100)); // 표준 라이브러리로 대체
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
};

AsyncDataRepository::AsyncDataRepository(IDataRepository* inner, bool ownInner)
    : m_pImpl(std::make_unique<Impl>(inner, ownInner)) // C++14 std::make_unique 사용
{
    // std::unique_ptr이라 내부 로우 포인터 접근 시 .get() 사용
    m_pImpl->m_worker = std::thread(&AsyncDataRepository::Impl::WorkerLoop, m_pImpl.get());
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
        // std::unique_ptr을 사용하므로 delete m_pImpl; 가 생략됩니다.
    }
}

StorageError AsyncDataRepository::SaveZFocusPoint(int runId, double zPosition, double score, int sampleCount, const std::string& extraJson)
{
    if(!m_pImpl) return StorageErrorWriteFailed;

    std::lock_guard<std::mutex> lock(m_pImpl->m_mutex);
    Impl::PendingZPoint pt;
    pt.runId = runId;
    pt.zPosition = zPosition;
    pt.score = score;
    pt.sampleCount = sampleCount;
    pt.extraJson = extraJson;
    
    m_pImpl->m_pointQueue.push_back(std::move(pt));
    m_pImpl->m_cv.notify_one();
    
    return StorageSuccess;
}

StorageError AsyncDataRepository::SaveZFocusResult(int camIndex, int locationId, int pkgId, double newFocus) 
{
    if(!m_pImpl) return StorageErrorWriteFailed;

    std::lock_guard<std::mutex> lock(m_pImpl->m_mutex);
    Impl::PendingResult res;
    res.camIndex = camIndex;
    res.locationId = locationId;
    res.pkgId = pkgId;
    res.newFocus = newFocus;

    m_pImpl->m_resultQueue.push_back(std::move(res));
    m_pImpl->m_cv.notify_one();

    return StorageSuccess;
}

StorageError AsyncDataRepository::UpdateSequenceRunStatus(int runId, const std::string& status, const std::string& resultSummaryJson) 
{
    if(!m_pImpl) return StorageErrorWriteFailed;

    std::lock_guard<std::mutex> lock(m_pImpl->m_mutex);
    Impl::PendingStatus st;
    st.runId = runId;
    st.status = status;
    st.resultSummaryJson = resultSummaryJson;

    m_pImpl->m_statusQueue.push_back(std::move(st));
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
