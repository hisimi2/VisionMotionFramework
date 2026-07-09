#include "pch.h"
#include "include/SampleActuatorAdapter.h"

namespace VMF_Sample
{
    using namespace VMF;

    // 축 인덱스 (pos 벡터 순서)
    // X=0, Y=1, Z=2, TABLE1=3, TABLE2=4

    static const char* AXIS_X = "X";
    static const char* AXIS_Y = "Y";
    static const char* AXIS_Z = "Z";
    static const char* AXIS_TABLE1 = "TABLE1";
    static const char* AXIS_TABLE2 = "TABLE2";

    // 생성자는 실제 장비 Parts 객체를 받아야 함
    SampleActuatorAdapter::SampleActuatorAdapter()
    {
    }

    SampleActuatorAdapter::~SampleActuatorAdapter()
    {
    }

    // 실제 장비의 PitchType을 반환
    VMF::PitchType SampleActuatorAdapter::GetPitchType()
    {
        // TODO: 장비의 실제 PitchType 반환 (예: m_parts->GetCurrentPitchType())
        return VMF::Variable;
    }

    // 장비의 안전 조건을 모두 검사
    VMF::ActError SampleActuatorAdapter::IsReadyToMove()
    {
        // TODO: 도어, 압력, Gripper 등 모든 안전 조건 확인 후 ActOk 반환
        return VMF::ActOk;
    }

    // 실제 Z축 이동 제어
    VMF::ActError SampleActuatorAdapter::MoveZ(double targetZ)
    {
        // TODO: m_parts->MoveAxis(AXIS_Z, targetZ, SPEED_HIGH) 호출 후 ActOk/ActError 반환
        return VMF::ActOk;
    }

    // 다축 이동 명령 처리
    VMF::ActError SampleActuatorAdapter::Move(VMF::MotionCommand& cmd)
    {
        // TODO: cmd에 포함된 각 축을 m_parts->MoveAxis() 로 이동
        return VMF::ActOk;
    }

    // Z축 목표 위치 도달 여부 확인
    VMF::ActError SampleActuatorAdapter::isMoveZ(double targetZ)
    {
        // TODO: m_parts->IsAxisInPosition(AXIS_Z, targetZ, TOL) 로 확인 후 ActOk/ActError 반환
        return VMF::ActOk;
    }

    // 다축 목표 위치 도달 여부 확인
    VMF::ActError SampleActuatorAdapter::isMove(VMF::MotionCommand& cmd)
    {
        // TODO: cmd에 포함된 각 축이 목표 위치에 도달했는지 확인
        return VMF::ActOk;
    }

    // 비상 정지/이동 중단
    VMF::ActError SampleActuatorAdapter::Stop()
    {
        // TODO: m_parts->StopAllAxes() 호출 후 ActOk 반환
        return VMF::ActOk;
    }

    // 현재 위치 반환
    std::vector<double> SampleActuatorAdapter::getPosition()
    {
        // TODO: m_parts->GetAxisPosition() 로 각 축 위치 반환
        return std::vector<double>();
    }

    // 현재 펄스 위치 반환
    std::vector<double> SampleActuatorAdapter::getPulse()
    {
        // TODO: m_parts->GetAxisPulse() 로 각 축 펄스 반환
        return std::vector<double>();
    }

    // 조명 제어 (켜기/끄기)
    int SampleActuatorAdapter::SetLightState(int cameraId, bool on)
    {
        // TODO: m_parts->TurnOnLight(cameraId) 또는 TurnOffLight(cameraId) 호출
        return 0; // 성공
    }

    // 조명 상태 조회
    int SampleActuatorAdapter::GetLightState(int camIndex, bool& outOn)
    {
        // TODO: m_parts->IsLightOn(camIndex) 로 상태 반환
        outOn = false;
        return 0;
    }

    // 레이저 제어 (설정)
    VMF::ActError SampleActuatorAdapter::SetLaserState(int laserChannel, bool on)
    {
        // TODO: 실제 레이저 제어 로직 구현
        return VMF::ActOk;
    }

    // 레이저 상태 조회
    VMF::ActError SampleActuatorAdapter::GetLaserState(int laserChannel, bool& outOn)
    {
        // TODO: 실제 레이저 상태 조회 로직 구현
        outOn = false;
        return VMF::ActOk;
    }

    // 트리거 제어 (설정)
    VMF::ActError SampleActuatorAdapter::SetTriggerState(bool enable, double intervalMm)
    {
        // TODO: 실제 트리거 제어 로직 구현
        return VMF::ActOk;
    }

    // 트리거 상태 조회
    VMF::ActError SampleActuatorAdapter::GetTriggerState(bool& outEnabled, double& outIntervalMm)
    {
        // TODO: 실제 트리거 상태 조회 로직 구현
        outEnabled = false;
        outIntervalMm = 0.0;
        return VMF::ActOk;
    }
} // namespace VMF_Sample
