#include "stdafx.h"
#include "Load1PickPlaceExample.h"
#include "Load1PickPlaceManager.h"
#include "Actuators/Load1Parts.h"
#include <iostream>

namespace AutoThread
{
    void Load1PickPlaceExample::BasicPickPlaceExample()
    {
        try
        {
            std::cout << "=== Basic Pick & Place Example ===" << std::endl;

            // 1. Load1Parts 생성
            Load1Parts parts;

            // 2. 매니저 생성
            auto manager = std::make_shared<Load1PickPlaceManager>();

            // 3. Pick & Place 3회 반복
            manager->Start(&parts, 3);

            std::cout << "상태: " << manager->GetStateString() << std::endl; // "Run"

            // 4. 완료까지 대기
            manager->WaitForCompletion();

            // 5. 결과 확인
            std::cout << "완료 반복 횟수: " << manager->GetCurrentIteration() << std::endl;
            std::cout << "성공 작업 수: " << manager->GetSuccessCount() << std::endl;

            if (!manager->GetLastError().empty())
            {
                std::cout << "에러: " << manager->GetLastError() << std::endl;
            }
        }
        catch (const std::exception& ex)
        {
            std::cerr << "Error: " << ex.what() << std::endl;
        }
    }

    void Load1PickPlaceExample::StateControlExample()
    {
        try
        {
            std::cout << "=== State Control Example ===" << std::endl;

            Load1Parts parts;
            auto manager = std::make_shared<Load1PickPlaceManager>();

            // 1. 시작 (Run 상태)
            manager->Start(&parts, 5);
            std::cout << "상태: " << manager->GetStateString() << std::endl; // "Run"

            // 2. 2초 후 일시 중지
            std::this_thread::sleep_for(std::chrono::seconds(2));
            manager->Pause();
            std::cout << "상태: " << manager->GetStateString() << std::endl; // "Stop"
            std::cout << "일시 중지된 반복 횟수: " << manager->GetCurrentIteration() << std::endl;

            // 3. 3초 대기 후 재개
            std::this_thread::sleep_for(std::chrono::seconds(3));
            manager->Resume();
            std::cout << "상태: " << manager->GetStateString() << std::endl; // "Run"

            // 4. 완료까지 대기
            manager->WaitForCompletion();

            std::cout << "최종 반복 횟수: " << manager->GetCurrentIteration() << std::endl;
            std::cout << "최종 성공 작업 수: " << manager->GetSuccessCount() << std::endl;
        }
        catch (const std::exception& ex)
        {
            std::cerr << "Error: " << ex.what() << std::endl;
        }
    }

    void Load1PickPlaceExample::InfinitePickPlaceExample()
    {
        try
        {
            std::cout << "=== Infinite Pick & Place Example ===" << std::endl;

            Load1Parts parts;
            auto manager = std::make_shared<Load1PickPlaceManager>();

            // 1. 무한 반복 시작 (repeatCount = 0)
            manager->Start(&parts, 0);

            // 2. 10초 동안 실행
            std::this_thread::sleep_for(std::chrono::seconds(10));

            // 3. 일시 중지
            manager->Pause();
            std::cout << "현재까지 완료된 반복 횟수: " << manager->GetCurrentIteration() << std::endl;

            // 4. 3초 대기
            std::this_thread::sleep_for(std::chrono::seconds(3));

            // 5. 종료
            manager->Terminate();

            std::cout << "최종 반복 횟수: " << manager->GetCurrentIteration() << std::endl;
            std::cout << "최종 성공 작업 수: " << manager->GetSuccessCount() << std::endl;
        }
        catch (const std::exception& ex)
        {
            std::cerr << "Error: " << ex.what() << std::endl;
        }
    }

    void Load1PickPlaceExample::MonitoringExample()
    {
        try
        {
            std::cout << "=== Monitoring Example ===" << std::endl;

            Load1Parts parts;
            auto manager = std::make_shared<Load1PickPlaceManager>();

            // 위치 설정
            manager->SetPickPosition(100.0, 200.0, -10.0);
            manager->SetPlacePosition(300.0, 150.0, -12.0);
            manager->SetSafeZ(5.0);
            manager->SetMoveTimeout(5000);

            // 시작
            manager->Start(&parts, 10);

            // 모니터링 루프
            while (!manager->IsComplete())
            {
                std::cout << "상태: " << manager->GetStateString()
                    << " | 반복: " << manager->GetCurrentIteration()
                    << " | 성공: " << manager->GetSuccessCount() << std::endl;

                std::this_thread::sleep_for(std::chrono::milliseconds(500));
            }

            std::cout << "작업 완료!" << std::endl;
            std::cout << "총 반복 횟수: " << manager->GetCurrentIteration() << std::endl;
            std::cout << "총 성공 작업: " << manager->GetSuccessCount() << std::endl;
        }
        catch (const std::exception& ex)
        {
            std::cerr << "Error: " << ex.what() << std::endl;
        }
    }
}
