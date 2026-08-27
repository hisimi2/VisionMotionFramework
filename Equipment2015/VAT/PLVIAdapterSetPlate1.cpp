#include "stdafx.h"
#include "PLVIAdapterSetPlate1.h"
#include "Actuators\SetPlate1Parts.h"

using namespace VMF;

PLVIAdapterSetPlate1::PLVIAdapterSetPlate1(SetPlate1Parts* parts)
    : m_parts(parts)
{
}

PLVIAdapterSetPlate1::~PLVIAdapterSetPlate1() {}


// ── 측정 위치 이동 (VisionPosition 기반) ──
ActError PLVIAdapterSetPlate1::MoveToMeasurementPosition(const VisionPosition& target)
{
    if (!m_parts) return ActError::ActFail;

    // Y축 이동 (스캔 방향)
    if (target.pos.size() > 1)
    {
        m_parts->AxisY.Move(target.pos[1]);
    }

    // Z축 이동 (Vision 촬영 높이)
    if (target.pos.size() > 2)
    {
        m_parts->AxisZ.Move(target.pos[2]);
    }

    return ActError::ActOk;
}

ActError PLVIAdapterSetPlate1::IsAtMeasurementPosition(const VisionPosition& target)
{
    if (!m_parts) return ActError::ActFail;

    // Y축 도착 확인
    if (target.pos.size() > 1)
    {
        double yDiff = std::abs(m_parts->AxisY.GetEncoder() - target.pos[1]);
        if (yDiff > 1.0)
            return ActError::ActWait;
    }

    // Z축 도착 확인
    if (target.pos.size() > 2)
    {
        double zDiff = std::abs(m_parts->AxisZ.GetEncoder() - target.pos[2]);
        if (zDiff > 1.0)
            return ActError::ActWait;
    }

    return ActError::ActOk;
}

// ── 홈 위치 이동 (VisionPosition 기반) ──
ActError PLVIAdapterSetPlate1::MoveToHomePosition(const VisionPosition& target)
{
    if (!m_parts) return ActError::ActFail;

    // 홈 위치 Y좌표 이동 (후퇴)
    if (target.pos.size() > 1)
    {
        m_parts->AxisY.Move(target.pos[1]);
    }

    // 홈 위치 Z좌표 이동 (안전 높이)
    if (target.pos.size() > 2)
    {
        m_parts->AxisZ.Move(target.pos[2]);
    }

    // 실린더 리프트 업
    if (m_parts->CylSetplateUpDown.size() > 0)
    {
        m_parts->CylSetplateUpDown[0].up(true);
    }

    return ActError::ActOk;
}

ActError PLVIAdapterSetPlate1::IsAtHomePosition(const VisionPosition& target)
{
    if (!m_parts) return ActError::ActFail;

    // Y축 도착 확인
    if (target.pos.size() > 1)
    {
        double yDiff = std::abs(m_parts->AxisY.GetEncoder() - target.pos[1]);
        if (yDiff > 1.0)
            return ActError::ActWait;
    }

    // Z축 도착 확인
    if (target.pos.size() > 2)
    {
        double zDiff = std::abs(m_parts->AxisZ.GetEncoder() - target.pos[2]);
        if (zDiff > 1.0)
            return ActError::ActWait;
    }

    // 실린더 리프트 업 확인
    if (m_parts->CylSetplateUpDown.size() > 0)
    {
        if (!m_parts->CylSetplateUpDown[0].isUp())
            return ActError::ActWait;
    }

    return ActError::ActOk;
}

// ── Z축 이동/확인 (VisionPosition 기반) ──
ActError PLVIAdapterSetPlate1::MoveToZ(const VisionPosition& target)
{
    if (!m_parts) return ActError::ActFail;
    double targetZ = 0.0;
    if (target.pos.size() > 2)
        targetZ = target.pos[2];
    m_parts->AxisZ.Move(targetZ);
    return ActError::ActOk;
}

ActError PLVIAdapterSetPlate1::IsAtZ(const VisionPosition& target)
{
    if (!m_parts) return ActError::ActFail;
    double targetZ = 0.0;
    if (target.pos.size() > 2)
        targetZ = target.pos[2];
    double diff = std::abs(m_parts->AxisZ.GetEncoder() - targetZ);
    return (diff <= 1.0) ? ActError::ActOk : ActError::ActWait;
}

// ── 안전 Z 이동/확인 (VisionPosition 기반) ──
ActError PLVIAdapterSetPlate1::MoveToZSafe(const VisionPosition& target)
{
    if (!m_parts) return ActError::ActFail;

    // 안전 Z 위치 이동
    double safeZ = 0.0;
    if (target.pos.size() > 2)
        safeZ = target.pos[2];

    m_parts->AxisZ.Move(safeZ);

    // 실린더 리프트 업 (안전 높이 확보)
    if (m_parts->CylSetplateUpDown.size() > 0)
    {
        m_parts->CylSetplateUpDown[0].up(true);
    }

    return ActError::ActOk;
}

ActError PLVIAdapterSetPlate1::IsAtZSafe(const VisionPosition& target)
{
    if (!m_parts) return ActError::ActFail;

    double safeZ = 0.0;
    if (target.pos.size() > 2)
        safeZ = target.pos[2];

    double zDiff = std::abs(m_parts->AxisZ.GetEncoder() - safeZ);
    if (zDiff > 1.0)
        return ActError::ActWait;

    // 실린더 리프트 업 확인
    if (m_parts->CylSetplateUpDown.size() > 0)
    {
        if (!m_parts->CylSetplateUpDown[0].isUp())
            return ActError::ActWait;
    }

    return ActError::ActOk;
}

// ── 조명/레이저 제어 ──
ActError PLVIAdapterSetPlate1::SetLightState(int camIndex, bool on, int lightIndex)
{
    (void)lightIndex;  // 현재는 camIndex만 사용
    if (!m_parts) return ActError::ActFail;
    switch (camIndex)
    {
    case 0: m_parts->LampPLVI.SetStatus(on); break;
    case 1: m_parts->LampSide.SetStatus(on); break;
    default: return ActError::ActFail;
    }
    return ActError::ActOk;
}

ActError PLVIAdapterSetPlate1::GetLightState(int camIndex, bool& outOn, int lightIndex)
{
    (void)lightIndex;  // 현재는 camIndex만 사용
    if (!m_parts) return ActError::ActFail;
    switch (camIndex)
    {
    case 0: outOn = m_parts->LampPLVI.GetStatus(); break;
    case 1: outOn = m_parts->LampSide.GetStatus(); break;
    default: return ActError::ActFail;
    }
    return ActError::ActOk;
}

ActError PLVIAdapterSetPlate1::SetTriggerState(bool enable, double intervalMm, int triggerIndex)
{
    (void)triggerIndex;  // 현재는 단일 트리거만 사용
    (void)enable;
    (void)intervalMm;
    if (!m_parts) return ActError::ActFail;

    // TODO: 실제 모션 컨트롤러 트리거 API 연결
    //
    // [AXT / 동양 계열 모션 컨트롤러 예시]
    // g_Motor[eAXIS_PLVI_MOD_1]->SetPlviCFG_REG(
    //     enable,
    //     m_scanDistMm  * 1000.0,   // 촬영 구간 (um 단위)
    //     m_accelDistMm * 1000.0,   // 가속 여유 거리 (um 단위)
    //     intervalMm    * 1000.0    // 트리거 간격 (um 단위)
    // );
    //
    // [DIO 보드 방식 예시 (NI DAQ 등)]
    // if (enable)
    // {
    //     m_parts->TriggerBoard.SetEncoderSource(AXIS_Y_ENCODER);
    //     m_parts->TriggerBoard.SetInterval(intervalMm * 1000.0);  // um
    //     m_parts->TriggerBoard.SetScanDist(m_scanDistMm * 1000.0);
    //     m_parts->TriggerBoard.SetAccelDist(m_accelDistMm * 1000.0);
    //     m_parts->TriggerBoard.Enable();
    // }
    // else
    // {
    //     m_parts->TriggerBoard.Disable();
    // }

    return ActError::ActOk;
}

ActError PLVIAdapterSetPlate1::GetTriggerState(bool& outEnabled, double& outIntervalMm, int triggerIndex)
{
    (void)triggerIndex;  // 현재는 단일 트리거만 사용
    outEnabled = false;
    outIntervalMm = 0.0;
    return ActError::ActOk;
}

// ── 레이저 제어 ──
ActError PLVIAdapterSetPlate1::SetLaserState(int laserChannel, bool on, int laserIndex)
{
    (void)laserIndex;  // 현재는 laserChannel만 사용
    if (!m_parts) return ActError::ActFail;
    // TODO: 실제 레이저 제어 구현
    // m_parts->Laser.SetStatus(on);
    return ActError::ActOk;
}

ActError PLVIAdapterSetPlate1::GetLaserState(int laserChannel, bool& outOn, int laserIndex)
{
    (void)laserIndex;  // 현재는 laserChannel만 사용
    if (!m_parts) return ActError::ActFail;
    // TODO: 실제 레이저 상태 조회 구현
    // outOn = m_parts->Laser.GetStatus();
    outOn = false;
    return ActError::ActOk;
}

// ── 준비/완료 동작 (VisionPosition 기반) ──
ActError PLVIAdapterSetPlate1::PrepareForInspection(const VisionPosition& target)
{
    if (!m_parts) return ActError::ActFail;

    // 실린더 클램프 (SetPlate 고정)
    if (m_parts->CylSetplateClamp.size() > 0)
    {
        m_parts->CylSetplateClamp[0].clamp(true);
    }

    // 실린더 리프트 업 (안전 높이 확보)
    if (m_parts->CylSetplateUpDown.size() > 0)
    {
        m_parts->CylSetplateUpDown[0].up(true);
    }

    // Z축 안전 위치 이동
    double safeZ = 0.0;
    if (target.pos.size() > 2)
        safeZ = target.pos[2];
    m_parts->AxisZ.Move(safeZ);

    return ActError::ActOk;
}

ActError PLVIAdapterSetPlate1::IsAtPrepareForInspection(const VisionPosition& target)
{
    if (!m_parts) return ActError::ActFail;

    // 실린더 클램프 확인
    if (m_parts->CylSetplateClamp.size() > 0)
    {
        if (!m_parts->CylSetplateClamp[0].isClamp())
            return ActError::ActWait;
    }

    // 실린더 리프트 업 확인
    if (m_parts->CylSetplateUpDown.size() > 0)
    {
        if (!m_parts->CylSetplateUpDown[0].isUp())
            return ActError::ActWait;
    }

    // Z축 위치 확인
    double safeZ = 0.0;
    if (target.pos.size() > 2)
        safeZ = target.pos[2];
    double zDiff = std::abs(m_parts->AxisZ.GetEncoder() - safeZ);
    if (zDiff > 1.0)
        return ActError::ActWait;

    return ActError::ActOk;
}

ActError PLVIAdapterSetPlate1::CompleteInspection(const VisionPosition& target)
{
    if (!m_parts) return ActError::ActFail;

    // 실린더 언클램프 (SetPlate 해제)
    if (m_parts->CylSetplateClamp.size() > 0)
    {
        m_parts->CylSetplateClamp[0].clamp(false);
    }

    // 실린더 리프트 다운
    if (m_parts->CylSetplateUpDown.size() > 0)
    {
        m_parts->CylSetplateUpDown[0].up(false);
    }

    // 홈 위치로 이동
    double homeY = 0.0;
    if (target.pos.size() > 1)
        homeY = target.pos[1];
    m_parts->AxisY.Move(homeY);

    return ActError::ActOk;
}

ActError PLVIAdapterSetPlate1::IsAtCompleteInspection(const VisionPosition& target)
{
    if (!m_parts) return ActError::ActFail;

    // 실린더 언클램프 확인
    if (m_parts->CylSetplateClamp.size() > 0)
    {
        if (m_parts->CylSetplateClamp[0].isClamp())
            return ActError::ActWait;
    }

    // 실린더 리프트 다운 확인
    if (m_parts->CylSetplateUpDown.size() > 0)
    {
        if (m_parts->CylSetplateUpDown[0].isUp())
            return ActError::ActWait;
    }

    // Y축 홈 위치 확인
    double homeY = 0.0;
    if (target.pos.size() > 1)
        homeY = target.pos[1];
    double yDiff = std::abs(m_parts->AxisY.GetEncoder() - homeY);
    if (yDiff > 1.0)
        return ActError::ActWait;

    return ActError::ActOk;
}


