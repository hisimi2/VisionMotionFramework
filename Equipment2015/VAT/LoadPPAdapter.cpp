#include "stdafx.h"
#include "LoadPPAdapter.h"

using namespace VMF;

// ── 생성자/소멸자 ──
LoadPPAdapter::LoadPPAdapter(Load1Parts* parts)
    : m_parts(parts)
{
}

LoadPPAdapter::~LoadPPAdapter()
{
}

// ── 측정 위치 이동 (VisionPosition 기반) ──
ActError LoadPPAdapter::MoveToMeasurementPosition(const VisionPosition& target)
{
    if (!m_parts) return ActError::ActFail;

    // X축 이동
    if (target.pos.size() > 0)
    {
        m_parts->AxisX.Move(target.pos[0]);
    }

    // Y축 이동
    if (target.pos.size() > 1)
    {
        m_parts->AxisY.Move(target.pos[1]);
    }

    // Z축 이동
    if (target.pos.size() > 2)
    {
        m_parts->AxisZ.Move(target.pos[2]);
    }

    // Table1축 이동
    if (target.pos.size() > 3)
    {
        m_parts->AxisTable1.Move(target.pos[3]);
    }

    // Table2축 이동
    if (target.pos.size() > 4)
    {
        m_parts->AxisTable2.Move(target.pos[4]);
    }

    return ActError::ActOk;
}

ActError LoadPPAdapter::IsAtMeasurementPosition(const VisionPosition& target)
{
    if (!m_parts) return ActError::ActFail;

    // X축 도착 확인
    if (target.pos.size() > 0)
    {
        double xDiff = std::abs(m_parts->AxisX.GetEncoder() - target.pos[0]);
        if (xDiff > 1.0)
            return ActError::ActWait;
    }

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

    // Table1축 도착 확인
    if (target.pos.size() > 3)
    {
        double t1Diff = std::abs(m_parts->AxisTable1.GetEncoder() - target.pos[3]);
        if (t1Diff > 1.0)
            return ActError::ActWait;
    }

    // Table2축 도착 확인
    if (target.pos.size() > 4)
    {
        double t2Diff = std::abs(m_parts->AxisTable2.GetEncoder() - target.pos[4]);
        if (t2Diff > 1.0)
            return ActError::ActWait;
    }

    return ActError::ActOk;
}

// ── 홈 위치 이동 (VisionPosition 기반) ──
ActError LoadPPAdapter::MoveToHomePosition(const VisionPosition& target)
{
    if (!m_parts) return ActError::ActFail;

    // Y축 후퇴
    if (target.pos.size() > 1)
    {
        m_parts->AxisY.Move(target.pos[1]);
    }

    // Z축 안전 높이 이동
    if (target.pos.size() > 2)
    {
        m_parts->AxisZ.Move(target.pos[2]);
    }

    // Table1축 대기 위치
    if (target.pos.size() > 3)
    {
        m_parts->AxisTable1.Move(target.pos[3]);
    }

    // Table2축 대기 위치
    if (target.pos.size() > 4)
    {
        m_parts->AxisTable2.Move(target.pos[4]);
    }

    // Buffer 실린더 후진 (안전 확보)
    m_parts->CylBuffer.backward(true);

    // Y축 Pitch 실린더 확장
    m_parts->CylYPitch.wide(true);

    return ActError::ActOk;
}

ActError LoadPPAdapter::IsAtHomePosition(const VisionPosition& target)
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

    // Table1축 도착 확인
    if (target.pos.size() > 3)
    {
        double t1Diff = std::abs(m_parts->AxisTable1.GetEncoder() - target.pos[3]);
        if (t1Diff > 1.0)
            return ActError::ActWait;
    }

    // Table2축 도착 확인
    if (target.pos.size() > 4)
    {
        double t2Diff = std::abs(m_parts->AxisTable2.GetEncoder() - target.pos[4]);
        if (t2Diff > 1.0)
            return ActError::ActWait;
    }

    // Buffer 실린더 후진 확인
    if (!m_parts->CylBuffer.isBackward())
        return ActError::ActWait;

    // Y축 Pitch 실린더 확장 확인
    if (!m_parts->CylYPitch.isWide())
        return ActError::ActWait;

    return ActError::ActOk;
}

// ── Z축 이동/확인 (VisionPosition 기반) ──
ActError LoadPPAdapter::MoveToZ(const VisionPosition& target)
{
    if (!m_parts) return ActError::ActFail;

    double targetZ = 0.0;
    if (target.pos.size() > 2)
        targetZ = target.pos[2];

    m_parts->AxisZ.Move(targetZ);
    return ActError::ActOk;
}

ActError LoadPPAdapter::IsAtZ(const VisionPosition& target)
{
    if (!m_parts) return ActError::ActFail;

    double targetZ = 0.0;
    if (target.pos.size() > 2)
        targetZ = target.pos[2];

    double zDiff = std::abs(m_parts->AxisZ.GetEncoder() - targetZ);
    return (zDiff <= 1.0) ? ActError::ActOk : ActError::ActWait;
}

// ── 안전 Z 이동/확인 (VisionPosition 기반) ──
ActError LoadPPAdapter::MoveToZSafe(const VisionPosition& target)
{
    if (!m_parts) return ActError::ActFail;

    // 안전 Z 위치 이동
    double safeZ = 0.0;
    if (target.pos.size() > 2)
        safeZ = target.pos[2];

    m_parts->AxisZ.Move(safeZ);

    // Buffer 실린더 후진 (안전 확보)
    m_parts->CylBuffer.backward(true);

    // Y축 Pitch 실린더 확장
    m_parts->CylYPitch.wide(true);

    return ActError::ActOk;
}

ActError LoadPPAdapter::IsAtZSafe(const VisionPosition& target)
{
    if (!m_parts) return ActError::ActFail;

    double safeZ = 0.0;
    if (target.pos.size() > 2)
        safeZ = target.pos[2];

    double zDiff = std::abs(m_parts->AxisZ.GetEncoder() - safeZ);
    if (zDiff > 1.0)
        return ActError::ActWait;

    // Buffer 실린더 후진 확인
    if (!m_parts->CylBuffer.isBackward())
        return ActError::ActWait;

    // Y축 Pitch 실린더 확장 확인
    if (!m_parts->CylYPitch.isWide())
        return ActError::ActWait;

    return ActError::ActOk;
}

// ── 조명/레이저 제어 ──
ActError LoadPPAdapter::SetLightState(int camIndex, bool on, int lightIndex)
{
    (void)lightIndex;
    if (!m_parts) return ActError::ActFail;

    switch (camIndex)
    {
    case 0: m_parts->LampLeft.SetStatus(on); break;
    case 1: m_parts->LampRight.SetStatus(on); break;
    case 2: m_parts->LampLower.SetStatus(on); break;
    default: return ActError::ActFail;
    }
    return ActError::ActOk;
}

ActError LoadPPAdapter::GetLightState(int camIndex, bool& outOn, int lightIndex)
{
    (void)lightIndex;
    if (!m_parts) return ActError::ActFail;

    switch (camIndex)
    {
    case 0: outOn = m_parts->LampLeft.GetStatus(); break;
    case 1: outOn = m_parts->LampRight.GetStatus(); break;
    case 2: outOn = m_parts->LampLower.GetStatus(); break;
    default: return ActError::ActFail;
    }
    return ActError::ActOk;
}

ActError LoadPPAdapter::SetLaserState(int laserChannel, bool on, int laserIndex)
{
    (void)laserIndex;
    (void)laserChannel;
    (void)on;
    // TODO: 실제 레이저 제어 구현
    return ActError::ActOk;
}

ActError LoadPPAdapter::GetLaserState(int laserChannel, bool& outOn, int laserIndex)
{
    (void)laserIndex;
    (void)laserChannel;
    outOn = false;
    // TODO: 실제 레이저 상태 조회 구현
    return ActError::ActOk;
}

// ── 트리거 제어 ──
ActError LoadPPAdapter::SetTriggerState(bool enable, double intervalMm, int triggerIndex)
{
    (void)triggerIndex;
    (void)enable;
    (void)intervalMm;
    // TODO: 실제 트리거 제어 구현
    return ActError::ActOk;
}

ActError LoadPPAdapter::GetTriggerState(bool& outEnabled, double& outIntervalMm, int triggerIndex)
{
    (void)triggerIndex;
    outEnabled = false;
    outIntervalMm = 0.0;
    // TODO: 실제 트리거 상태 조회 구현
    return ActError::ActOk;
}

// ── 준비/완료 동작 (VisionPosition 기반) ──
ActError LoadPPAdapter::PrepareForInspection(const VisionPosition& target)
{
    if (!m_parts) return ActError::ActFail;

    // Buffer 실린더 후진 (안전 확보)
    m_parts->CylBuffer.backward(true);

    // Y축 Pitch 실린더 확장
    m_parts->CylYPitch.wide(true);

    // Z축 안전 위치 이동
    double safeZ = 0.0;
    if (target.pos.size() > 2)
        safeZ = target.pos[2];
    m_parts->AxisZ.Move(safeZ);

    return ActError::ActOk;
}

ActError LoadPPAdapter::IsAtPrepareForInspection(const VisionPosition& target)
{
    if (!m_parts) return ActError::ActFail;

    // Buffer 실린더 후진 확인
    if (!m_parts->CylBuffer.isBackward())
        return ActError::ActWait;

    // Y축 Pitch 실린더 확장 확인
    if (!m_parts->CylYPitch.isWide())
        return ActError::ActWait;

    // Z축 위치 확인
    double safeZ = 0.0;
    if (target.pos.size() > 2)
        safeZ = target.pos[2];
    double zDiff = std::abs(m_parts->AxisZ.GetEncoder() - safeZ);
    if (zDiff > 1.0)
        return ActError::ActWait;

    return ActError::ActOk;
}

ActError LoadPPAdapter::CompleteInspection(const VisionPosition& target)
{
    if (!m_parts) return ActError::ActFail;

    // Buffer 실린더 전진
    m_parts->CylBuffer.backward(false);

    // Y축 Pitch 실린더 수축
    m_parts->CylYPitch.wide(false);

    // 홈 위치로 Y축 이동
    double homeY = 0.0;
    if (target.pos.size() > 1)
        homeY = target.pos[1];
    m_parts->AxisY.Move(homeY);

    return ActError::ActOk;
}

ActError LoadPPAdapter::IsAtCompleteInspection(const VisionPosition& target)
{
    if (!m_parts) return ActError::ActFail;

    // Buffer 실린더 전진 확인
    if (m_parts->CylBuffer.isBackward())
        return ActError::ActWait;

    // Y축 Pitch 실린더 수축 확인
    if (m_parts->CylYPitch.isWide())
        return ActError::ActWait;

    // Y축 홈 위치 확인
    double homeY = 0.0;
    if (target.pos.size() > 1)
        homeY = target.pos[1];
    double yDiff = std::abs(m_parts->AxisY.GetEncoder() - homeY);
    if (yDiff > 1.0)
        return ActError::ActWait;

    return ActError::ActOk;
}


