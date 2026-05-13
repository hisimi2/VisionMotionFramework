#pragma once

#include "IDataRepository.h"
#include <mutex> 

namespace VMF
{
    class VMF_API FileDataRepository : public IDataRepository
    {
    public:
        /**
         * @brief 생성자.
         * @param basePath 데이터 저장소의 기본 경로.
         */
        explicit FileDataRepository(const std::string& basePath);

        /**
         * @brief 소멸자.
         */
        ~FileDataRepository() override;

        /**
         * @brief 파라미터를 저장합니다.
         * @param recipe 레시피 이름.
         * @param name 파라미터 이름.
         * @param value 저장할 값.
         * @return StorageError 성공 여부.
         */
        StorageError SaveParam(const std::string& recipe, const std::string& name, const std::string& value) override;
        StorageError LoadParam(const std::string& recipe, const std::string& name, std::string& outValue) override;
        StorageError SaveImage(const std::string& contextTag, const std::vector<uint8_t>& imageData, std::string& outPath) override;
        StorageError SaveSequenceRun(const std::string& sequenceName, const std::string& summary) override;

        /**
         * @brief 저장소 초기화.
         * @return StorageError 성공 여부.
         */
        StorageError Initialize() override;

        /**
         * @brief 저장소 종료.
         * @return StorageError 성공 여부.
         */
        StorageError Shutdown() override;

        /* --- IDataRepository 확장(구조화 API) --- */
        // sequence_runs 레코드 생성: paramsJson은 JSON 문자열, outRunId는 생성된 run id 반환
        StorageError CreateSequenceRun(const std::string& sequenceName, const std::string& paramsJson, int& outRunId) override;

        // z-focus 스캔 포인트 저장 (run_id 연계)
        StorageError SaveZFocusPoint(int runId, double zPosition, double score, int sampleCount, const std::string& extraJson) override;

        // z-focus 결과 저장 (run 당 1개)
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

		StorageError SaveTeachingResult(
			int handId,
			int locationId,
			int pkgId,
			double posX,
			double posY,
			double posZ) override;

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
			double& narrowRight,
			double& narrowLeft,
			double& wideRight,
			double& wideLeft) override;


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

        // sequence_runs 상태 업데이트
        StorageError UpdateSequenceRunStatus(int runId, const std::string& status, const std::string& resultSummaryJson) override;

    private:
        std::string basePath_;
        mutable std::mutex mutex_;
    };

}; // namespace VMF
