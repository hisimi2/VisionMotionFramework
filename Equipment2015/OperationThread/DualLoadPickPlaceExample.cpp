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

    
}
