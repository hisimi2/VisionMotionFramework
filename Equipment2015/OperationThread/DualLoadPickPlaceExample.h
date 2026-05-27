#pragma once

namespace OperationThread
{
    class DualLoadPickPlaceExample
{
    public:
 /// <summary>
        /// 기본 Load1 + Load2 동시 Pick & Place 예제
      /// startSwitch 상태에 따라 두 시퀀스가 동시에 제어됨
        /// </summary>
       static void BasicDualPickPlaceExample();

        /// <summary>
   /// Load1 + Load2 모니터링 예제
        /// startSwitch 상태 변화에 따른 Pause/Resume을 모니터링
        /// </summary>
 static void DualMonitoringExample();

        /// <summary>
     /// Load1 + Load2 스위치 제어 예제
  /// startSwitch를 수동으로 ON/OFF하여 시퀀스 제어
        /// </summary>
        static void DualSwitchControlExample();

/// <summary>
  /// Load1 + Load2 무한 반복 예제
     /// startSwitch 상태에 따라 무한 반복 중 Pause/Resume
   /// </summary>
    static void DualInfinitePickPlaceExample();
    };
}
