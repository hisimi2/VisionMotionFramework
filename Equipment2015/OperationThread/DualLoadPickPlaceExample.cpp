#include "stdafx.h"
#include "DualLoadPickPlaceExample.h"
#include "DualLoadPickPlaceController.h"
#include "Actuators/COPSwitch.h"
#include "Actuators/Load1Parts.h"
#include "Actuators/Load2Parts.h"
#include <iostream>
#include <thread>
#include <chrono>

namespace OperationThread
{
    void DualLoadPickPlaceExample::BasicDualPickPlaceExample()
    {
        try
    {
     std::cout << "=== Basic Dual Load Pick & Place Example ===" << std::endl;

          // 1. 스위치 생성 (초기값: ON)
      COPSwitch startSwitch("StartSwitch");
   startSwitch.setStatus(true);

       // 2. Load Parts 생성
     Load1Parts load1Parts;
          Load2Parts load2Parts;

        // 3. 통합 컨트롤러 생성
   auto controller = std::make_shared<DualLoadPickPlaceController>(&startSwitch);

            // 4. Load1과 Load2 시작 (3회 반복)
            controller->Start(&load1Parts, &load2Parts, 3);

      std::cout << "Load1 상태: " << controller->GetLoad1StateString() << std::endl;
  std::cout << "Load2 상태: " << controller->GetLoad2StateString() << std::endl;

      // 5. 2초 후 스위치 OFF (Pause)
            std::this_thread::sleep_for(std::chrono::seconds(2));
      std::cout << "\n>>> 스위치 OFF - 두 시퀀스 일시 중지" << std::endl;
       startSwitch.setStatus(false);

std::this_thread::sleep_for(std::chrono::seconds(2));
          std::cout << "Load1 반복: " << controller->GetLoad1CurrentIteration()
           << " | Load2 반복: " << controller->GetLoad2CurrentIteration() << std::endl;

  // 6. 3초 후 스위치 ON (Resume)
        std::cout << "\n>>> 스위치 ON - 두 시퀀스 재개" << std::endl;
 startSwitch.setStatus(true);

     std::this_thread::sleep_for(std::chrono::seconds(3));
  std::cout << "Load1 반복: " << controller->GetLoad1CurrentIteration()
        << " | Load2 반복: " << controller->GetLoad2CurrentIteration() << std::endl;

 // 7. 완료까지 대기
            while (!controller->IsBothComplete())
          {
     std::this_thread::sleep_for(std::chrono::milliseconds(500));
            }

            // 8. 결과 출력
       std::cout << "\n=== 결과 ===" << std::endl;
  std::cout << "Load1 완료 반복: " << controller->GetLoad1CurrentIteration() << std::endl;
            std::cout << "Load1 성공: " << controller->GetLoad1SuccessCount() << std::endl;
   std::cout << "Load2 완료 반복: " << controller->GetLoad2CurrentIteration() << std::endl;
            std::cout << "Load2 성공: " << controller->GetLoad2SuccessCount() << std::endl;

        controller->Stop();
        }
        catch (const std::exception& ex)
   {
      std::cerr << "Error: " << ex.what() << std::endl;
        }
    }

    void DualLoadPickPlaceExample::DualMonitoringExample()
    {
      try
 {
      std::cout << "=== Dual Load Monitoring Example ===" << std::endl;

            // 1. 스위치 및 Parts 생성
    COPSwitch startSwitch("StartSwitch");
        Load1Parts load1Parts;
    Load2Parts load2Parts;

  // 2. 통합 컨트롤러 생성 및 시작
    auto controller = std::make_shared<DualLoadPickPlaceController>(&startSwitch);
            controller->Start(&load1Parts, &load2Parts, 5);

          // 3. 모니터링 루프
            int monitorCount = 0;
          bool switchToggle = true;

          while (!controller->IsBothComplete() && monitorCount < 50)  // 최대 50번 모니터링
          {
     // 3초마다 스위치 토글
        if (monitorCount % 30 == 0 && monitorCount > 0)
       {
  switchToggle = !switchToggle;
 startSwitch.setStatus(switchToggle);
      std::cout << "\n>>> 스위치 " << (switchToggle ? "ON" : "OFF") << std::endl;
      }

           std::cout << "모니터링 #" << monitorCount
 << " | Load1: " << controller->GetLoad1StateString()
    << " (반복: " << controller->GetLoad1CurrentIteration() << ")"
              << " | Load2: " << controller->GetLoad2StateString()
         << " (반복: " << controller->GetLoad2CurrentIteration() << ")" << std::endl;

     std::this_thread::sleep_for(std::chrono::milliseconds(100));
 ++monitorCount;
  }

            std::cout << "\n=== 최종 결과 ===" << std::endl;
            std::cout << "Load1: " << controller->GetLoad1CurrentIteration()
          << " / 성공: " << controller->GetLoad1SuccessCount() << std::endl;
            std::cout << "Load2: " << controller->GetLoad2CurrentIteration()
            << " / 성공: " << controller->GetLoad2SuccessCount() << std::endl;

            controller->Stop();
        }
        catch (const std::exception& ex)
    {
     std::cerr << "Error: " << ex.what() << std::endl;
        }
    }

    void DualLoadPickPlaceExample::DualSwitchControlExample()
    {
        try
        {
      std::cout << "=== Dual Load Switch Control Example ===" << std::endl;

            // 1. 스위치 및 Parts 생성
  COPSwitch startSwitch("StartSwitch");
        Load1Parts load1Parts;
 Load2Parts load2Parts;

    // 2. 통합 컨트롤러 생성 및 시작
    auto controller = std::make_shared<DualLoadPickPlaceController>(&startSwitch);

     // 3. 초기 ON 상태로 시작
      startSwitch.setStatus(true);
            controller->Start(&load1Parts, &load2Parts, 10);

      std::cout << "초기 상태: 스위치 ON, 시퀀스 시작" << std::endl;

         // 4. 패턴별 스위치 제어
    for (int cycle = 0; cycle < 3 && !controller->IsBothComplete(); ++cycle)
       {
    // ON 상태 유지 (2초)
      std::cout << "\n사이클 " << (cycle + 1) << ": ON 상태 (2초)" << std::endl;
     std::this_thread::sleep_for(std::chrono::seconds(2));
 std::cout << "Load1: " << controller->GetLoad1CurrentIteration()
         << " | Load2: " << controller->GetLoad2CurrentIteration() << std::endl;

          // OFF 상태로 변경 (Pause)
         std::cout << "스위치 OFF (Pause)" << std::endl;
      startSwitch.setStatus(false);
      std::this_thread::sleep_for(std::chrono::seconds(1));

      // ON 상태로 복구 (Resume)
         std::cout << "스위치 ON (Resume)" << std::endl;
      startSwitch.setStatus(true);
  std::this_thread::sleep_for(std::chrono::seconds(1));
   }

 // 5. 완료까지 대기
        while (!controller->IsBothComplete())
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
          }

     std::cout << "\n=== 작업 완료 ===" << std::endl;
 std::cout << "Load1 최종: " << controller->GetLoad1CurrentIteration()
     << " | Load2 최종: " << controller->GetLoad2CurrentIteration() << std::endl;

    controller->Stop();
        }
        catch (const std::exception& ex)
        {
    std::cerr << "Error: " << ex.what() << std::endl;
        }
    }

    void DualLoadPickPlaceExample::DualInfinitePickPlaceExample()
    {
        try
        {
  std::cout << "=== Dual Load Infinite Pick & Place Example ===" << std::endl;

     // 1. 스위치 및 Parts 생성
            COPSwitch startSwitch("StartSwitch");
          Load1Parts load1Parts;
            Load2Parts load2Parts;

      // 2. 통합 컨트롤러 생성 및 무한 반복 시작
   auto controller = std::make_shared<DualLoadPickPlaceController>(&startSwitch);
    startSwitch.setStatus(true);
       controller->Start(&load1Parts, &load2Parts, 0);  // 0 = 무한 반복

     std::cout << "무한 반복 모드로 시작 (스위치로 제어)" << std::endl;

        // 3. 10초 동안 실행
   std::this_thread::sleep_for(std::chrono::seconds(10));

  // 4. Pause
   std::cout << "\n10초 경과 - 스위치 OFF (Pause)" << std::endl;
      startSwitch.setStatus(false);
            std::cout << "Load1 반복: " << controller->GetLoad1CurrentIteration() << std::endl;
      std::cout << "Load2 반복: " << controller->GetLoad2CurrentIteration() << std::endl;

  // 5. 3초 대기
       std::this_thread::sleep_for(std::chrono::seconds(3));

   // 6. Resume
    std::cout << "\n3초 대기 후 - 스위치 ON (Resume)" << std::endl;
        startSwitch.setStatus(true);

            // 7. 5초 더 실행
            std::this_thread::sleep_for(std::chrono::seconds(5));

  // 8. 종료
   std::cout << "\n5초 추가 실행 후 - 시퀀스 종료" << std::endl;
       std::cout << "Load1 최종 반복: " << controller->GetLoad1CurrentIteration() << std::endl;
        std::cout << "Load2 최종 반복: " << controller->GetLoad2CurrentIteration() << std::endl;
      std::cout << "Load1 성공: " << controller->GetLoad1SuccessCount() << std::endl;
      std::cout << "Load2 성공: " << controller->GetLoad2SuccessCount() << std::endl;

    controller->Stop();
        }
     catch (const std::exception& ex)
   {
            std::cerr << "Error: " << ex.what() << std::endl;
        }
    }
}
