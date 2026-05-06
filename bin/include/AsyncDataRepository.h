#pragma once

#include "IDataRepository.h"

namespace DVH_VAT 
{

    /**
     * @class AsyncDataRepository
     * @brief IDataRepository 구현체를 비동기 큐로 래핑하여 Pimpl 패턴을 적용합니다.
     */
    class DVH_VAT_API AsyncDataRepository : public IDataRepository 
    {
    public:
        AsyncDataRepository(IDataRepository* inner, bool ownInner);
        virtual ~AsyncDataRepository();

        // IDataRepository 인터페이스
        virtual StorageError SaveParam(const std::string& recipe, const std::string& name, const std::string& value);
        virtual StorageError LoadParam(const std::string& recipe, const std::string& name, std::string& outValue);
        virtual StorageError SaveImage(const std::string& contextTag, const std::vector<uint8_t>& imageData, std::string& outPath);
        virtual StorageError SaveSequenceRun(const std::string& sequenceName, const std::string& summary);
        virtual StorageError Initialize();
        virtual StorageError Shutdown();
        virtual StorageError CreateSequenceRun(const std::string& sequenceName, const std::string& paramsJson, int& outRunId);

        // 비동기 처리 대상
        virtual StorageError SaveZFocusPoint(int runId, double zPosition, double score, int sampleCount, const std::string& extraJson);
        virtual StorageError SaveZFocusResult(int camIndex, int locationId, int pkgId, double newFocus);
        virtual StorageError UpdateSequenceRunStatus(int runId, const std::string& status, const std::string& resultSummaryJson);

        // Picker-Cam 거리 값 저장
        virtual StorageError SavePickerCamDistanceResult(int camIndex, int pkgId, double narrowX, double narrowY, double wideX, double wideY);

        // 검사 위치 정보 저장
        virtual StorageError SaveCalibrationPosResult(int camIndex, int locationId, int pkgId, double posX, double posY);

        // 핸드 피치 정보 저장
        virtual StorageError SaveHandPitchResult(int handId, int pkgId, int col, int row, double narrowX, double narrowY, double wideX, double wideY);

        virtual StorageError SaveTeachingResult(int handId, int locationId, int pkgId, double posX, double posY, double posZ);

        //////////////검사 결과 로드//////////////////
        StorageError LoadInspInitPos(int camIndex, int locationId, int pkgId, double& posX, double& posY, double& focus);
        StorageError LoadPickerCamDistance(int camIndex, int pkgId, double& narrowRight, double& narrowLeft, double& wideRight, double& wideLeft);
        StorageError LoadHandPitch(int handId, int pkgId, int row, int col, double& narrowX, double& narrowY, double& wideX, double& wideY);
        StorageError LoadTeachingResult(int handId, int locationId, int pkgId, int dateoffset, double& posX, double& posY, double& posZ);
        StorageError LoadHandCamGroup(int handId, std::vector<int>& camIds);
        StorageError LoadCamLocationGroup(int camIndex, std::vector<int>& locateIds);
        StorageError LoadLocationIdByName(const std::string& locateName, int& locationId);

    private:
        // Pimpl 관용구: 구현부 구조체 전방 선언
        struct Impl;
    
        // v100 호환성을 위해 일반 포인터 사용 (소멸자에서 delete 필수)
        Impl* m_pImpl;
    };

} // namespace DVH_VAT
