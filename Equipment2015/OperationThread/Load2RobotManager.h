#pragma once

#include <memory>
#include <string>


#include "SequenceManager.h"

class Load2Parts;


namespace OperationThread
{
    class Load2RobotSequence;
    using Load2RobotSequencePtr = std::shared_ptr<Load2RobotSequence>;

    /// <summary>
    /// Load2 Pick & Place 매니저
    /// EquipmentCore의 SequenceManager를 사용하여 Load2 Pick & Place 작업 관리
    /// </summary>
    class Load2RobotManager
    {
    public:
        explicit Load2RobotManager();
        ~Load2RobotManager();

        /// <summary>
        /// Pick & Place 작업 시작
        /// </summary>
        /// <param name="parts">Load2Parts 객체</param>
        /// <param name="repeatCount">반복 횟수 (0 = 무한 반복)</param>
        void Start(Load2Parts* parts, int repeatCount = 1);

        /// <summary>
        /// 작업 일시 중지
        /// </summary>
        void Pause();

        /// <summary>
        /// 작업 재개
        /// </summary>
        void Resume();

        /// <summary>
        /// 작업 완전 종료
        /// </summary>
        void Terminate();

        /// <summary>
        /// 상태 문자열 반환
        /// </summary>
        std::string GetStateString() const;

        /// <summary>
        /// 작업 완료 여부 확인
        /// </summary>
        bool IsComplete() const;

        /// <summary>
        /// 마지막 오류 메시지
        /// </summary>
        std::string GetLastError() const;

        /// <summary>
        /// 작업 완료까지 대기
        /// </summary>
        void WaitForCompletion();

        /// <summary>
        /// Pick 위치 설정
        /// </summary>
        void SetPickPosition(double x, double z);

        /// <summary>
        /// Place 위치 설정
        /// </summary>
        void SetPlacePosition(double x, double z);

        /// <summary>
        /// Safe Z 설정
        /// </summary>
        void SetSafeZ(double z);

        /// <summary>
        /// 타임아웃 설정
        /// </summary>
        void SetMoveTimeout(long timeoutMs);

        /// <summary>
        /// 현재 반복 횟수
        /// </summary>
        int GetCurrentIteration() const;

        /// <summary>
        /// 성공한 작업 수
        /// </summary>
        int GetSuccessCount() const;

    private:
        EC::SequenceManagerPtr m_manager;
        Load2RobotSequencePtr m_sequence;
        Load2Parts* m_parts;
    };

    using Load2RobotManagerPtr = std::shared_ptr<Load2RobotManager>;
}
