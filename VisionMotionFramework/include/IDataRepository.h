#pragma once
#include "VMF_API.h"

#include "Types.h"
#include <cstdint>
#include <string>
#include <vector>

namespace VMF
{
    /**
     * @brief 데이터 저장소 오류 코드.
     * 모든 저장소 구현체는 이 오류 코드를 사용하여 연산 결과를 반환합니다.
     */
    enum StorageError
    {
        StorageSuccess = 0,
        StorageErrorWriteFailed,
        StorageErrorNotFound,
        StorageWriteFailed,
        StorageFileNotFound,
        StorageNotFound,
        StorageGeneral,
        StorageNotImplemented = -100  // 확장 메서드 기본 반환값
    };

    /**
     * @brief 검사 유형 코드.
     * 프로토콜 기반 검사 유형을 식별하기 위한 열거형입니다.
     * 새 검사 유형 추가 시 여기에 값을 추가하거나, 문자열 기반 type_code를 사용할 수 있습니다.
     */
    enum class InspectionType
    {
        Unknown = 0,
        PLVI,                    // PLVI 검사 (Pocket Leave, Device Missing 등)
        Orientation,             // Orientation Check (A1 index mark)
        COK_ID,                  // COK ID 검사
        VAT,                     // Vision Auto Teaching
        Piggyback,               // Piggyback 검사 (Empty/Exist Check)
        BallInspection,          // Ball Inspection
        SetCok,                  // SET_COK 설정
        // 기존 검사 유형 (하위 호환)
        ZFocus,
        CalibrationPos,
        PickerCamDistance,
        HandPitch,
        Teaching
    };

    /**
     * @brief 검사 결과 상세 항목.
     * inspection_results 테이블에 저장되는 개별 결과 항목을 표현합니다.
     */
    struct InspectionResultItem
    {
        int runId;                    // inspection_runs.id
        std::string resultType;       // 결과 유형 (예: "device_result", "socket_result", "offset_data")
        int resultIndex;              // 배열 인덱스 (-1이면 인덱스 없음)
        std::string resultJson;       // 세부 결과 데이터 (JSON 형태)
    };

    /**
     * @brief 검사 위치 정보.
     * InspectionLocation 테이블 조회 결과를 표현합니다.
     */
    struct InspectionLocationInfo
    {
        int locationCode;
        std::string locationName;
        std::string locationType;
        bool isActive;
    };

    /**
     * @brief Socket 마스터 정보.
     * Socket_Masters 테이블 조회 결과를 표현합니다.
     */
    struct SocketMasterInfo
    {
        int socketNo;
        int socketType;        // 0=Single, 1=Dual-H, 2=Dual-V
        int stationNo;
    };

    /**
     * @brief 데이터 저장소의 핵심 인터페이스.
     *
     * VMF의 모든 데이터 저장소 구현체는 이 인터페이스를 구현해야 합니다.
     * 기존 IDataRepository와 IDataRepositoryExtension을 통합하여,
     * 검사 유형별 메서드 추가 없이 범용 저장/로드가 가능하도록 설계되었습니다.
     *
     * @note 검사 결과 저장은 inspection_runs + inspection_results 구조로 통합되며,
     *       검사 유형별 테이블 추가 없이 메타데이터 기반으로 확장 가능합니다.
     */
    class VMF_API IDataRepository
    {
    public:
        virtual ~IDataRepository() = default;

        // ─────────────────────────────────────────────
        // 핵심 메서드 (모든 구현체 필수)
        // ─────────────────────────────────────────────

        /**
         * @brief 기본 키-값 스타일 파라미터 저장.
         * @param recipe 레시피 이름.
         * @param name 파라미터 이름.
         * @param value 파라미터 값.
         * @return StorageSuccess 또는 오류 코드.
         */
        virtual StorageError SaveParam(const std::string& recipe, const std::string& name, const std::string& value) = 0;

        /**
         * @brief 기본 키-값 스타일 파라미터 로드.
         * @param recipe 레시피 이름.
         * @param name 파라미터 이름.
         * @param outValue 로드된 값 (성공 시).
         * @return StorageSuccess, StorageNotFound 등.
         */
        virtual StorageError LoadParam(const std::string& recipe, const std::string& name, std::string& outValue) = 0;

        /**
         * @brief 이미지 저장: 파일에 쓰고 경로를 outPath로 반환.
         * @param contextTag 이미지 컨텍스트 태그 (파일명 생성에 사용).
         * @param imageData 이미지 바이너리 데이터.
         * @param outPath 저장된 파일 경로 (성공 시).
         * @return StorageSuccess 또는 오류 코드.
         */
        virtual StorageError SaveImage(const std::string& contextTag, const std::vector<uint8_t>& imageData, std::string& outPath) = 0;

        /**
         * @brief 저장소 초기화.
         * @return StorageSuccess 또는 오류 코드.
         */
        virtual StorageError Initialize() = 0;

        /**
         * @brief 저장소 종료/정리.
         * @return StorageSuccess 또는 오류 코드.
         */
        virtual StorageError Shutdown() = 0;

        // ─────────────────────────────────────────────
        // 검사 실행 이력 관리 (범용)
        // ─────────────────────────────────────────────

        /**
         * @brief 검사 실행 이력 생성.
         * @param type 검사 유형 (InspectionType).
         * @param runId 프로토콜 nDataID 또는 시퀀스 run_id.
         * @param paramsJson 요청 파라미터 전체 (JSON 형태, cData 등).
         * @param outDbRunId 생성된 DB 레코드 ID (성공 시).
         * @return StorageSuccess 또는 오류 코드.
         *
         * @note inspection_runs 테이블에 레코드를 삽입하고, outDbRunId에 DB ID를 반환합니다.
         *       type_code는 InspectionType을 문자열로 변환하여 저장합니다.
         */
        virtual StorageError CreateInspectionRun(InspectionType type, const std::string& runId,
            const std::string& paramsJson, int& outDbRunId) = 0;

        /**
         * @brief 검사 실행 이력 상태 업데이트 및 결과 저장.
         * @param dbRunId inspection_runs.id.
         * @param status 상태 ("running", "finished", "error" 등).
         * @param resultJson 결과 데이터 전체 (JSON 형태, cData 결과 등).
         * @param errorCode 프로토콜 Error Description 코드 (-1이면 없음).
         * @return StorageSuccess 또는 오류 코드.
         *
         * @note status가 "finished"이면 finished_at이 자동 설정됩니다.
         *       result_json에 검사 유형별 결과 데이터를 저장합니다.
         */
        virtual StorageError UpdateInspectionRunStatus(int dbRunId, const std::string& status,
            const std::string& resultJson, int errorCode = -1) = 0;

        /**
         * @brief 검사 실행 이력 로드.
         * @param dbRunId inspection_runs.id.
         * @param outTypeCode 검사 유형 코드 문자열 (예: "PLVI", "VAT").
         * @param outRunId 프로토콜 run_id.
         * @param outStatus 상태.
         * @param outResultJson 결과 데이터 JSON.
         * @param outErrorCode 오류 코드 (-1이면 없음).
         * @return StorageSuccess, StorageNotFound 등.
         */
        virtual StorageError LoadInspectionRun(int dbRunId, std::string& outTypeCode, std::string& outRunId,
            std::string& outStatus, std::string& outResultJson, int& outErrorCode) = 0;

        // ─────────────────────────────────────────────
        // 검사 결과 상세 저장/로드 (범용)
        // ─────────────────────────────────────────────

        /**
         * @brief 검사 결과 상세 항목 저장.
         * @param dbRunId inspection_runs.id.
         * @param resultType 결과 유형 (예: "device_result", "socket_result", "offset_data").
         * @param resultIndex 배열 인덱스 (Device 0, Socket 1 등, 인덱스 없으면 -1).
         * @param resultJson 세부 결과 데이터 (JSON 형태).
         * @return StorageSuccess 또는 오류 코드.
         *
         * @note inspection_results 테이블에 레코드를 삽입합니다.
         *       검사 유형별 필드 차이는 resultJson에 JSON으로 저장합니다.
         */
        virtual StorageError SaveInspectionResult(int dbRunId, const std::string& resultType,
            int resultIndex, const std::string& resultJson) = 0;

        /**
         * @brief 검사 결과 상세 항목 로드.
         * @param dbRunId inspection_runs.id.
         * @param outResults 로드된 결과 항목 목록.
         * @return StorageSuccess, StorageNotFound 등.
         *
         * @note inspection_results 테이블의 모든 항목을 로드하여 outResults에 반환합니다.
         *       resultType, resultIndex 기준으로 정렬됩니다.
         */
        virtual StorageError LoadInspectionResults(int dbRunId, std::vector<InspectionResultItem>& outResults) = 0;

        // ─────────────────────────────────────────────
        // 시퀀스 실행 이력 (기존 호환)
        // ─────────────────────────────────────────────

        /**
         * @brief 시퀀스 실행 이력 생성 (기존 호환).
         * @param sequenceName 시퀀스 이름.
         * @param paramsJson 파라미터 JSON.
         * @param outRunId 생성된 시퀀스 run_id.
         * @return StorageSuccess 또는 오류 코드.
         *
         * @note sequence_runs 테이블에 레코드를 삽입합니다.
         *       기존 CreateSequenceRun과의 호환성을 위해 유지됩니다.
         */
        virtual StorageError CreateSequenceRun(const std::string& sequenceName, const std::string& paramsJson, int& outRunId) = 0;

        /**
         * @brief 시퀀스 실행 상태 업데이트 (기존 호환).
         * @param runId sequence_runs.id.
         * @param status 상태.
         * @param resultSummaryJson 결과 요약 JSON.
         * @return StorageSuccess 또는 오류 코드.
         */
        virtual StorageError UpdateSequenceRunStatus(int runId, const std::string& status,
            const std::string& resultSummaryJson) = 0;

        // ─────────────────────────────────────────────
        // Z-포커스 관련 (기존 호환, 점진적 마이그레이션 대상)
        // ─────────────────────────────────────────────

        /**
         * @brief Z-포커스 측정점 저장 (기존 호환).
         * @param runId 시퀀스 run_id.
         * @param zPosition Z 위치.
         * @param score 점수.
         * @param sampleCount 샘플 수.
         * @param extraJson 추가 정보 JSON.
         * @return StorageSuccess 또는 오류 코드.
         *
         * @note z_focus_points 테이블에 저장합니다.
         *       새 검사 유형은 CreateInspectionRun + SaveInspectionResult를 사용하는 것을 권장합니다.
         */
        virtual StorageError SaveZFocusPoint(int runId, double zPosition, double score,
            int sampleCount, const std::string& extraJson) = 0;

        /**
         * @brief Z-포커스 결과 저장 (기존 호환).
         * @param camIndex 카메라 인덱스.
         * @param locationId 위치 ID.
         * @param pkgId 패키지 ID.
         * @param newFocus 새로운 포커스 값.
         * @return StorageSuccess 또는 오류 코드.
         *
         * @note InspInitPos 테이블에 focus를 갱신합니다.
         *       새 검사 유형은 CreateInspectionRun + SaveInspectionResult를 사용하는 것을 권장합니다.
         */
        virtual StorageError SaveZFocusResult(int camIndex, int locationId, int pkgId, double newFocus) = 0;

        /**
         * @brief Z-포커스 결과 로드 (기존 호환).
         * @param camIndex 카메라 인덱스.
         * @param locationId 위치 ID.
         * @param pkgId 패키지 ID.
         * @param outPosX 위치 X.
         * @param outPosY 위치 Y.
         * @param outFocus 포커스 값.
         * @return StorageSuccess, StorageNotFound 등.
         */
        virtual StorageError LoadInspInitPos(int camIndex, int locationId, int pkgId,
            double& outPosX, double& outPosY, double& outFocus) = 0;

        // ─────────────────────────────────────────────
        // 피커-캠 거리 관련 (기존 호환, 점진적 마이그레이션 대상)
        // ─────────────────────────────────────────────

        /**
         * @brief 피커-캠 거리 결과 저장 (기존 호환).
         * @param camIndex 카메라 인덱스.
         * @param pkgId 패키지 ID.
         * @param narrowX Narrow 오프셋 X.
         * @param narrowY Narrow 오프셋 Y.
         * @param wideX Wide 오프셋 X.
         * @param wideY Wide 오프셋 Y.
         * @return StorageSuccess 또는 오류 코드.
         *
         * @note PickerCamDistance 테이블에 저장합니다.
         *       새 검사 유형은 CreateInspectionRun + SaveInspectionResult를 사용하는 것을 권장합니다.
         */
        virtual StorageError SavePickerCamDistanceResult(int camIndex, int pkgId,
            double narrowX, double narrowY, double wideX, double wideY) = 0;

        /**
         * @brief 피커-캠 거리 결과 로드 (기존 호환).
         * @param camIndex 카메라 인덱스.
         * @param pkgId 패키지 ID.
         * @param outNarrowX Narrow 오프셋 X.
         * @param outNarrowY Narrow 오프셋 Y.
         * @param outWideX Wide 오프셋 X.
         * @param outWideY Wide 오프셋 Y.
         * @return StorageSuccess, StorageNotFound 등.
         */
        virtual StorageError LoadPickerCamDistance(int camIndex, int pkgId,
            double& outNarrowX, double& outNarrowY, double& outWideX, double& outWideY) = 0;

        // ─────────────────────────────────────────────
        // 캘리브레이션 위치 관련 (기존 호환, 점진적 마이그레이션 대상)
        // ─────────────────────────────────────────────

        /**
         * @brief 캘리브레이션 위치 결과 저장 (기존 호환).
         * @param camIndex 카메라 인덱스.
         * @param locationId 위치 ID.
         * @param pkgId 패키지 ID.
         * @param posX 위치 X.
         * @param posY 위치 Y.
         * @return StorageSuccess 또는 오류 코드.
         *
         * @note InspInitPos 테이블에 pos_x, pos_y를 갱신합니다.
         *       새 검사 유형은 CreateInspectionRun + SaveInspectionResult를 사용하는 것을 권장합니다.
         */
        virtual StorageError SaveCalibrationPosResult(int camIndex, int locationId, int pkgId,
            double posX, double posY) = 0;

        // ─────────────────────────────────────────────
        // 핸드 피치 관련 (기존 호환, 점진적 마이그레이션 대상)
        // ─────────────────────────────────────────────

        /**
         * @brief 핸드 피치 결과 저장 (기존 호환).
         * @param handId 핸드 ID.
         * @param pkgId 패키지 ID.
         * @param row 행.
         * @param col 열.
         * @param narrowX Narrow 오프셋 X.
         * @param narrowY Narrow 오프셋 Y.
         * @param wideX Wide 오프셋 X.
         * @param wideY Wide 오프셋 Y.
         * @return StorageSuccess 또는 오류 코드.
         *
         * @note HandPitch 테이블에 저장합니다.
         *       새 검사 유형은 CreateInspectionRun + SaveInspectionResult를 사용하는 것을 권장합니다.
         */
        virtual StorageError SaveHandPitchResult(int handId, int pkgId, int row, int col,
            double narrowX, double narrowY, double wideX, double wideY) = 0;

        /**
         * @brief 핸드 피치 결과 로드 (기존 호환).
         * @param handId 핸드 ID.
         * @param pkgId 패키지 ID.
         * @param row 행.
         * @param col 열.
         * @param outNarrowX Narrow 오프셋 X.
         * @param outNarrowY Narrow 오프셋 Y.
         * @param outWideX Wide 오프셋 X.
         * @param outWideY Wide 오프셋 Y.
         * @return StorageSuccess, StorageNotFound 등.
         */
        virtual StorageError LoadHandPitch(int handId, int pkgId, int row, int col,
            double& outNarrowX, double& outNarrowY, double& outWideX, double& outWideY) = 0;

        // ─────────────────────────────────────────────
        // 티칭 결과 관련 (기존 호환, 점진적 마이그레이션 대상)
        // ─────────────────────────────────────────────

        /**
         * @brief 티칭 결과 저장 (기존 호환).
         * @param handId 핸드 ID.
         * @param locationId 위치 ID.
         * @param pkgId 패키지 ID.
         * @param posX 위치 X.
         * @param posY 위치 Y.
         * @param posZ 위치 Z.
         * @return StorageSuccess 또는 오류 코드.
         *
         * @note UpperCamTeachingInspection + TeachingPos 테이블에 저장합니다.
         *       새 검사 유형은 CreateInspectionRun + SaveInspectionResult를 사용하는 것을 권장합니다.
         */
        virtual StorageError SaveTeachingResult(int handId, int locationId, int pkgId,
            double posX, double posY, double posZ) = 0;

        /**
         * @brief 티칭 결과 로드 (기존 호환).
         * @param handId 핸드 ID.
         * @param locationId 위치 ID.
         * @param pkgId 패키지 ID.
         * @param dateoffset 날짜 오프셋 (0 = 최신).
         * @param outPosX 위치 X.
         * @param outPosY 위치 Y.
         * @param outPosZ 위치 Z.
         * @return StorageSuccess, StorageNotFound 등.
         */
        virtual StorageError LoadTeachingResult(int handId, int locationId, int pkgId, int dateoffset,
            double& outPosX, double& outPosY, double& outPosZ) = 0;

        // ─────────────────────────────────────────────
        // 그룹 매핑 관련 (기존 호환, 점진적 마이그레이션 대상)
        // ─────────────────────────────────────────────

        /**
         * @brief 핸드-카메라 그룹 로드 (기존 호환).
         * @param handId 핸드 ID.
         * @param outCamIds 카메라 인덱스 목록.
         * @return StorageSuccess, StorageNotFound 등.
         */
        virtual StorageError LoadHandCamGroup(int handId, std::vector<int>& outCamIds) = 0;

        /**
         * @brief 카메라-위치 그룹 로드 (기존 호환).
         * @param camIndex 카메라 인덱스.
         * @param outLocateIds 위치 ID 목록.
         * @return StorageSuccess, StorageNotFound 등.
         */
        virtual StorageError LoadCamLocationGroup(int camIndex, std::vector<int>& outLocateIds) = 0;

        /**
         * @brief 위치명 → 위치 ID 로드 (기존 호환).
         * @param locateName 위치명.
         * @param outLocationId 위치 ID.
         * @return StorageSuccess, StorageNotFound 등.
         */
        virtual StorageError LoadLocationIdByName(const std::string& locateName, int& outLocationId) = 0;

        // ─────────────────────────────────────────────
        // 검사 위치 마스터 (신규)
        // ─────────────────────────────────────────────

        /**
         * @brief 검사 위치 마스터 로드.
         * @param locationCode 위치 코드 (0=Set-Plate1, 1=Set-Plate2, ..., 11=Shuttle Table1, ...).
         * @param outLocationName 위치명.
         * @param outLocationType 위치 유형 ("SetPlate", "ShuttleTable", "RetestBuffer" 등).
         * @return StorageSuccess, StorageNotFound 등.
         */
        virtual StorageError LoadInspectionLocation(int locationCode, std::string& outLocationName,
            std::string& outLocationType) = 0;

        /**
         * @brief 검사 위치 마스터 목록 로드.
         * @param outLocations 위치 목록.
         * @return StorageSuccess 또는 오류 코드.
         */
        virtual StorageError LoadAllInspectionLocations(std::vector<InspectionLocationInfo>& outLocations) = 0;

        // ─────────────────────────────────────────────
        // Socket 마스터 (신규)
        // ─────────────────────────────────────────────

        /**
         * @brief Socket 마스터 로드.
         * @param socketNo Socket 번호 (1~32).
         * @param outSocketType Socket 유형 (0=Single, 1=Dual-H, 2=Dual-V).
         * @param outStationNo Station 번호.
         * @return StorageSuccess, StorageNotFound 등.
         */
        virtual StorageError LoadSocketMaster(int socketNo, int& outSocketType, int& outStationNo) = 0;

        /**
         * @brief Socket 마스터 목록 로드.
         * @param outSockets Socket 목록.
         * @return StorageSuccess 또는 오류 코드.
         */
        virtual StorageError LoadAllSocketMasters(std::vector<SocketMasterInfo>& outSockets) = 0;
    };

} // namespace VMF

