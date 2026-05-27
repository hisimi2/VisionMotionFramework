#pragma once

#include <chrono>
#include <string>
#include <memory>

class Load2Parts;

namespace OperationThread
{
  /// <summary>
    /// Load2 Pick & Place 시퀀스 구현
    /// </summary>
  class Load2PickPlaceSequence
    {
    public:
    enum class PickPlaceStep
        {
       RailOpen = 0,
        MovePickPositionXZ,
         PreciserDown,
 VacuumOn,
        MoveSafeZAfterPick,
MovePlacePositionXZ,
 PreciserUp,
            VacuumOff,
  MoveSafeZAfterPlace,
        CheckRepeat,
      Complete
   };

        Load2PickPlaceSequence(Load2Parts* parts, int repeatCount = 1);
        ~Load2PickPlaceSequence();

        // 시퀀스 제어 메서드
        void OnInitialize();
        bool OnPoll();
        void OnCleanup();
        void OnError(const std::string& errorMsg);

        // 파라미터 설정 메서드
        void SetPickPosition(double x, double z);
        void SetPlacePosition(double x, double z);
        void SetSafeZ(double z);
        void SetMoveTimeout(long timeoutMs);
        void SetVacuumIndex(int index);

      // 상태 조회 메서드
      PickPlaceStep GetCurrentStep() const;
      int GetCurrentIteration() const;
        int GetSuccessCount() const;

    private:
 // 단계별 처리 함수
        bool HandleRailOpen();
        bool HandleMovePickPositionXZ();
        bool HandlePreciserDown();
        bool HandleVacuumOn();
        bool HandleMoveSafeZAfterPick();
 bool HandleMovePlacePositionXZ();
        bool HandlePreciserUp();
     bool HandleVacuumOff();
        bool HandleMoveSafeZAfterPlace();
     bool HandleCheckRepeat();
        bool HandleComplete();

      // 유틸리티 메서드
        void MoveToNextStep();
        bool IsStepTimeout() const;
void LogStep(const std::string& message);

        // 멤버 변수
        Load2Parts* m_parts;
        int m_repeatCount;
        int m_currentIteration;
        int m_successCount;
        PickPlaceStep m_currentStep;
        long m_moveTimeoutMs;

    // 위치 파라미터
        double m_pickX, m_pickZ;
        double m_placeX, m_placeZ;
  double m_safeZ;

 // 액추에이터 인덱스
        int m_vacuumIndex;

        // 상태
        bool m_initialized;
        std::string m_lastError;
        std::chrono::steady_clock::time_point m_stepStartTime;
    };

} // namespace OperationThread
