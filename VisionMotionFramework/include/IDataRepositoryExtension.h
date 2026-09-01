#pragma once

#include "IDataRepository.h"
#include <string>
#include <vector>

namespace VMF
{
    /// <summary>
    /// 데이터 저장소의 확장 인터페이스.
    /// 구조화된 검사 결과 저장/로드를 위한 메서드들을 제공합니다.
    /// 기본 구현은 StorageNotImplemented를 반환하며, 필요 시 구현체에서 오버라이드합니다.
    /// </summary>
    class VMF_API IDataRepositoryExtension
    {
    public:
        virtual ~IDataRepositoryExtension() = default;

        // ── 시퀀스 실행 관련 ──
        virtual StorageError SaveSequenceRun(const std::string& sequenceName, const std::string& summary)
        {
            return StorageNotImplemented;
        }

        virtual StorageError CreateSequenceRun(const std::string& sequenceName, const std::string& paramsJson, int& outRunId)
        {
            outRunId = -1;
            return StorageNotImplemented;
        }

        virtual StorageError UpdateSequenceRunStatus(int runId, const std::string& status, const std::string& resultSummaryJson)
        {
            return StorageNotImplemented;
        }

        // ── z-focus 관련 ──
        virtual StorageError SaveZFocusPoint(int runId, double zPosition, double score, int sampleCount, const std::string& extraJson)
        {
            return StorageNotImplemented;
        }

        virtual StorageError SaveZFocusResult(int camIndex, int locationId, int pkgId, double newFocus)
        {
            return StorageNotImplemented;
        }

        // ── 검사 결과 저장 ──
        virtual StorageError SavePickerCamDistanceResult(int camIndex, int pkgId, double narrowX, double narrowY, double wideX, double wideY)
        {
            return StorageNotImplemented;
        }

        virtual StorageError SaveCalibrationPosResult(int camIndex, int locationId, int pkgId, double posX, double posY)
        {
            return StorageNotImplemented;
        }

        virtual StorageError SaveHandPitchResult(int handId, int pkgId, int col, int row, double narrowX, double narrowY, double wideX, double wideY)
        {
            return StorageNotImplemented;
        }

        virtual StorageError SaveTeachingResult(int handId, int locationId, int pkgId, double posX, double posY, double posZ)
        {
            return StorageNotImplemented;
        }

        // ── 검사 결과 로드 ──
        virtual StorageError LoadInspInitPos(int camIndex, int locationId, int pkgId, double& posX, double& posY, double& focus)
        {
            posX    = 0.0;
            posY    = 0.0;
            focus   = 0.0;
            return StorageNotFound;
        }

        virtual StorageError LoadPickerCamDistance(int camIndex, int pkgId, double& narrowX, double& narrowY, double& wideX, double& wideY)
        {
            narrowX = 0.0;
            narrowY = 0.0;
            wideX   = 0.0;
            wideY   = 0.0;
            return StorageNotFound;
        }

        virtual StorageError LoadHandPitch(int handId, int pkgId, int row, int col, double& narrowX, double& narrowY, double& wideX, double& wideY)
        {
            narrowX = 0.0;
            narrowY = 0.0;
            wideX = 0.0;
            wideY = 0.0;
            return StorageNotFound;
        }

        virtual StorageError LoadTeachingResult(int handId, int locationId, int pkgId, int dateoffset, double& posX, double& posY, double& posZ)
        {
            posX = 0.0;
            posY = 0.0;
            posZ = 0.0;
            return StorageNotFound;
        }

        virtual StorageError LoadHandCamGroup(int handId, std::vector<int>& camIds)
        {
            camIds.clear();
            return StorageNotFound;
        }

        virtual StorageError LoadCamLocationGroup(int camIndex, std::vector<int>& locateIds)
        {
            locateIds.clear();
            return StorageNotFound;
        }

        virtual StorageError LoadLocationIdByName(const std::string& locateName, int& locationId)
        {
            locationId = -1;
            return StorageNotFound;
        }
    };
} // namespace VMF
