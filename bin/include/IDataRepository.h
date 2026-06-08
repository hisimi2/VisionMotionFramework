#pragma once
#include "VMF_API.h"

#include "Types.h"
#include <cstdint> // <stdint.h> 대신 C++ 표준 헤더 사용
#include <string>
#include <vector>

namespace VMF 
{
    enum StorageError 
    {
        StorageSuccess = 0,
        StorageErrorWriteFailed,
        StorageErrorNotFound, 
        StorageWriteFailed,
        StorageFileNotFound,
        StorageNotFound,
        StorageGeneral
    };

    class VMF_API IDataRepository {
    public:
        virtual ~IDataRepository() = default;

        // 기본 키-값 스타일 저장/로드
        virtual StorageError SaveParam(const std::string& recipe, const std::string& name, const std::string& value) = 0;
        virtual StorageError LoadParam(const std::string& recipe, const std::string& name, std::string& outValue) = 0;

        // 이미지 저장: 파일에 쓰고 경로를 outPath로 반환
        virtual StorageError SaveImage(const std::string& contextTag, const std::vector<uint8_t>& imageData, std::string& outPath) = 0;

        // 간단한 시퀀스 요약 저장
        virtual StorageError SaveSequenceRun(const std::string& sequenceName, const std::string& summary) = 0;

        // 초기화/종료
        virtual StorageError Initialize() = 0;
        virtual StorageError Shutdown() = 0;

        // sequence_runs 레코드 생성: paramsJson은 JSON을 문자열로 전달, outRunId는 생성된 run id 반환
        virtual StorageError CreateSequenceRun(const std::string& sequenceName, const std::string& paramsJson, int& outRunId) = 0;

        // z-focus 스캔 포인트 저장 (run_id 연계)
        virtual StorageError SaveZFocusPoint(int runId, double zPosition, double score, int sampleCount, const std::string& extraJson) = 0;


        //////////////////// 검사 결과 저장/////////////////////
        
        // z-focus 결과 저장 (run 당 1개)
        virtual StorageError SaveZFocusResult(int camIndex, int locationId, int pkgId, double newFocus) = 0;

        // Picker-Cam 거리 값 저장
        virtual StorageError SavePickerCamDistanceResult(int camIndex, int pkgId, double narrowX, double narrowY, double wideX, double wideY) = 0;

        // 검사 위치 정보 저장
        virtual StorageError SaveCalibrationPosResult(int camIndex, int locationId, int pkgId, double posX, double posY) = 0;
        
        // sequence_runs 상태 업데이트
        virtual StorageError UpdateSequenceRunStatus(int runId, const std::string& status, const std::string& resultSummaryJson) = 0;

        // 핸드 피치 정보 저장
		virtual StorageError SaveHandPitchResult(
			int handId,
			int pkgId,
			int col,
			int row,
			double narrowX,
			double narrowY,
			double wideX,
			double wideY) = 0;

        // Teaching 검사 결과 저장
        virtual StorageError SaveTeachingResult(int handId, int locationId, int pkgId, double posX, double posY, double posZ) = 0;


		//////////////////// 검사 결과 로드/////////////////////
		virtual StorageError LoadInspInitPos(
			int camIndex,
			int locationId,
			int pkgId,
			double& posX,
			double& posY,
			double& focus) = 0;


		virtual StorageError LoadPickerCamDistance(
			int camIndex,
			int pkgId,
            double& narrowX,
            double& narrowY,
            double& wideX,
            double& wideY) = 0;


		virtual StorageError LoadHandPitch(
			int handId,
			int pkgId,
			int row,
			int col,
			double& narrowX,
			double& narrowY,
			double& wideX,
			double& wideY) = 0;


		// offset: 0=최신, 1=그 다음, 2=그 다음...
		virtual StorageError LoadTeachingResult(
			int handId,
			int locationId,
			int pkgId,
			int dateoffset,
			double& posX,
			double& posY,
			double& posZ) = 0;

		virtual StorageError LoadHandCamGroup(
			int handId,
			std::vector<int>& camIds) = 0;

        virtual StorageError LoadCamLocationGroup(
            int camIndex,
            std::vector<int>& locateIds) = 0;


		virtual StorageError LoadLocationIdByName(
			const std::string& locateName,
			int& locationId) = 0;

    };

} // namespace VMF
