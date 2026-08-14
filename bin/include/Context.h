#pragma once
#include "VMF_API.h"
#include "Types.h"

#include "IVisionClient.h"
#include "IDataRepository.h"
#include "IParamProvider.h"

#include <cstdlib>
#include <sstream>
#include <memory> // std::enable_shared_from_this 사용 목적
#include <mutex>  // std::mutex 사용 목적

namespace VMF
{
    class VMF_API Context : public std::enable_shared_from_this<Context>, public IParamProvider
    {
    public:
        /// <summary>
        /// VAT 실행 컨텍스트를 초기화합니다.
        /// </summary>
        Context();

        /// <summary>
        /// VAT 실행 컨텍스트를 정리합니다.
        /// </summary>
        ~Context();

        /// <summary>
        /// 비전 명령 실행에 사용할 비전 프로세서 인터페이스를 설정합니다.
        /// </summary>
        void SetVisionProcessor(VisionProcessorPtr vp);

        /// <summary>
        /// 현재 등록된 비전 프로세서 인터페이스를 반환합니다.
        /// </summary>
        VisionProcessorPtr GetVisionProcessorInterface() const;

        /// <summary>
        /// 데이터 조회 및 저장에 사용할 저장소 인터페이스를 설정합니다.
        /// </summary>
        void SetDataRepository(DataRepositoryPtr repo);

        /// <summary>
        /// 현재 등록된 데이터 저장소 인터페이스를 반환합니다.
        /// </summary>
        DataRepositoryPtr GetRepository() const;

        /// <summary>
        /// 마지막 오류 메시지를 저장합니다.
        /// </summary>
        void SetLastError(const std::string& error);

        /// <summary>
        /// 마지막으로 저장된 오류 메시지를 반환합니다.
        /// </summary>
        std::string GetLastError() const;

        /// <summary>
        /// 작업 중지 요청 상태를 설정합니다.
        /// </summary>
        void SetStopRequested(bool stop);

        /// <summary>
        /// 현재 작업 중지 요청 상태를 반환합니다.
        /// </summary>
        bool GetStopRequested() const;

        /// <summary>
        /// 현재 저장된 비전 파라미터를 사용하여 지정한 비전 명령을 실행합니다.
        /// </summary>
        bool ExecuteVisionCommand(VisionCommand cmd);

        // ── Task 파라미터 관리 ──
        /// <summary>
        /// Task별 파라미터를 설정합니다.
        /// SequenceBuilder가 시퀀스 실행 전에 필요한 파라미터를 설정합니다.
        /// Task가 다양해질 것을 대비하여 제네릭 StringMap 기반 파라미터 저장소를 사용합니다.
        /// </summary>
        void SetTaskParams(const TaskParams& params);

        /// <summary>
        /// Task별 파라미터를 반환합니다.
        /// Task가 Context를 통해 파라미터를 읽어올 때 사용합니다.
        /// </summary>
        TaskParams GetTaskParams() const;

        /// <summary>
        /// 실행 파라미터 조회 (문자열)
        /// </summary>
        std::string GetExecutionParam(const std::string& key) const override;

        /// <summary>
        /// 실행 파라미터 설정 (문자열)
        /// </summary>
        void SetExecutionParam(const std::string& key, const std::string& value) override;

        /// <summary>
        /// 실행 파라미터 설정 (정수)
        /// </summary>
        void SetExecutionParam(const std::string& key, int value) override;

        /// <summary>
        /// 실행 파라미터 설정 (실수)
        /// </summary>
        void SetExecutionParam(const std::string& key, double value) override;

        /// <summary>
        /// VisionPositions 목록을 조회합니다.
        /// </summary>
        std::vector<VisionPosition> GetVisionPositions() const override;

        /// <summary>
        /// 마지막 VisionPosition을 조회합니다.
        /// </summary>
        bool PeekVisionPosition(VisionPosition& outPos) const override;

private:
        VisionProcessorPtr      m_processor;
        DataRepositoryPtr       m_repo;
        TaskParams              m_taskParams;

        mutable std::mutex      m_mutex;
        std::string             m_lastError;
        bool                    m_isStopRequested;
    };
} // namespace VMF
