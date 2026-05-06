#pragma once

#include "IDataRepository.h"

namespace DVH_VAT
{
    class DVH_VAT_API FileDataRepository : public IDataRepository
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
        virtual ~FileDataRepository();

        /**
         * @brief 파라미터를 저장합니다.
         * @param recipe 레시피 이름.
         * @param name 파라미터 이름.
         * @param value 저장할 값.
         * @return StorageError 성공 여부.
         */
        virtual StorageError SaveParam(const std::string& recipe, const std::string& name, const std::string& value);
        virtual StorageError LoadParam(const std::string& recipe, const std::string& name, std::string& outValue);
        virtual StorageError SaveImage(const std::string& contextTag, const std::vector<uint8_t>& imageData, std::string& outPath);
        virtual StorageError SaveSequenceRun(const std::string& sequenceName, const std::string& summary);

        /**
         * @brief 저장소 초기화.
         * @return StorageError 성공 여부.
         */
        virtual StorageError Initialize();

        /**
         * @brief 저장소 종료.
         * @return StorageError 성공 여부.
         */
        virtual StorageError Shutdown();

        /* --- IDataRepository 확장(구조화 API) --- */
        // sequence_runs 레코드 생성: paramsJson은 JSON 문자열, outRunId는 생성된 run id 반환
        virtual StorageError CreateSequenceRun(const std::string& sequenceName, const std::string& paramsJson, int& outRunId);

        // z-focus 스캔 포인트 저장 (run_id 연계)
        virtual StorageError SaveZFocusPoint(int runId, double zPosition, double score, int sampleCount, const std::string& extraJson);

        // z-focus 결과 저장 (run 당 1개)
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

		virtual StorageError SaveTeachingResult(
			int handId,
			int locationId,
			int pkgId,
			double posX,
			double posY,
			double posZ);

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
			double& narrowRight,
			double& narrowLeft,
			double& wideRight,
			double& wideLeft);


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

        // sequence_runs 상태 업데이트
        virtual StorageError UpdateSequenceRunStatus(int runId, const std::string& status, const std::string& resultSummaryJson);

    private:
        std::string basePath_;
        
        // [v100] boost::mutex 등 호환 타입 (CompatUtils.h 정의)
        mutable boost::mutex mutex_;
    };

}; // namespace DVH_VAT
