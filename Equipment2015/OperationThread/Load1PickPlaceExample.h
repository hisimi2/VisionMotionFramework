#pragma once

namespace OperationThread
{
    /// <summary>
    /// Load1 Pick & Place 상태머신 사용 예제
    /// </summary>
    class Load1PickPlaceExample
    {
    public:
        /// <summary>
        /// 기본 Pick & Place 반복 예제
        /// </summary>
        static void BasicPickPlaceExample();

        /// <summary>
        /// 상태 제어 예제 (Run -> Pause -> Resume -> Exit)
        /// </summary>
        static void StateControlExample();

        /// <summary>
        /// 무한 반복 Pick & Place 예제
        /// </summary>
        static void InfinitePickPlaceExample();

        /// <summary>
        /// 실시간 모니터링 예제
        /// </summary>
        static void MonitoringExample();
    };
}
