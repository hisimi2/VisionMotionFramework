#include "stdafx.h"
#include "PLVIAdapterSetPlate1.h"

// SetPlate1 PLVI 축 구성
//   Y축 : 스캔 방향 (PLVI 촬영 이동)
//   Z축 : 안전 높이 / Vision 촬영 높이 제어

namespace VMF
{
    static const char* AXIS_Y = "Y";
    static const char* AXIS_Z = "Z";

    PLVIAdapterSetPlate1::PLVIAdapterSetPlate1(SetPlate1Parts* parts)
        : m_parts(parts)
        , m_triggerEnabled(false)
        , m_triggerIntervalMm(0.0)
        , m_scanDistMm(315.0)    // SetPlate1 기준 (Shuttle=532.0)
        , m_accelDistMm(10.0)    // 가속 여유 거리 (모든 검사 위치에서 공통적으로 10 사용)
    {
        // 기본 위치값 초기화
        m_startPos.pos = { 0.0, 0.0, 0.0 };
        m_scanEndPos.pos = { 0.0, 200.0, 0.0 };
        m_safeZPos.pos = { 0.0, 0.0, 0.0 };
        m_homePos.pos = { 0.0, 0.0, 0.0 };
    }

    PLVIAdapterSetPlate1::~PLVIAdapterSetPlate1() {}

    PitchType PLVIAdapterSetPlate1::GetPitchType() { return Fixed; }
    ActError  PLVIAdapterSetPlate1::IsReadyToMove() { return m_parts ? ActOk : ActFail; }

    // ── Z축 이동 (VisionPosition 기반) ──
    ActError PLVIAdapterSetPlate1::MoveZ(const VisionPosition& target)
    {
        if (!m_parts) return ActFail;
        double targetZ = 0.0;
        if (target.pos.size() > 2)
            targetZ = target.pos[2];
        m_parts->AxisZ.Move(targetZ);
        return ActOk;
    }

    ActError PLVIAdapterSetPlate1::isMoveZ(const VisionPosition& target)
    {
        if (!m_parts) return ActFail;
        double targetZ = 0.0;
        if (target.pos.size() > 2)
            targetZ = target.pos[2];
        double diff = std::abs(m_parts->AxisZ.GetEncoder() - targetZ);
        return (diff <= 1.0) ? ActOk : ActWait;
    }

    // ── 일반 축 이동 ──
    ActError PLVIAdapterSetPlate1::Move(MotionCommand& cmd)
    {
        if (!m_parts) return ActFail;

        if (cmd.Exist(AXIS_Y))
        {
            // 스캔 속도 설정
            // if (cmd.Get(AXIS_Y).value().speed.has_value())
            //     m_parts->AxisY.SetSpeed(cmd.Get(AXIS_Y).value().speed.value());

            // TODO: 실제 Y축 이동 구현
            // double targetY = cmd.Get(AXIS_Y).value().targetPos;
            // m_parts->AxisY.Move(targetY);
        }

        return ActOk;
    }

    ActError PLVIAdapterSetPlate1::isMove(MotionCommand& cmd)
    {
        if (!m_parts) return ActFail;

        if (cmd.Exist(AXIS_Y))
        {
            // TODO: 실제 Y축 도착 확인 구현
            // double target = cmd.Get(AXIS_Y).value().targetPos;
            // if (std::abs(m_parts->AxisY.GetEncoder() - target) > 1.0)
            //     return ActWait;
        }

        return ActOk;
    }

    ActError PLVIAdapterSetPlate1::Stop()
    {
        if (!m_parts) return ActFail;
        // m_parts->AxisY.Stop();
        // m_parts->AxisZ.Stop();
        return ActOk;
    }

    // ── 측정 위치 이동 (VisionPosition 기반) ──
    ActError PLVIAdapterSetPlate1::MoveToMeasurementPosition(const VisionPosition& target)
    {
        if (!m_parts) return ActFail;

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

        return ActOk;
    }

    ActError PLVIAdapterSetPlate1::IsAtMeasurementPosition(const VisionPosition& target)
    {
        if (!m_parts) return ActFail;

        // Y축 도착 확인
        if (target.pos.size() > 1)
        {
            double yDiff = std::abs(m_parts->AxisY.GetEncoder() - target.pos[1]);
            if (yDiff > 1.0)
                return ActWait;
        }

        // Z축 도착 확인
        if (target.pos.size() > 2)
        {
            double zDiff = std::abs(m_parts->AxisZ.GetEncoder() - target.pos[2]);
            if (zDiff > 1.0)
                return ActWait;
        }

        return ActOk;
    }

    // ── 홈 위치 이동 (VisionPosition 기반) ──
    ActError PLVIAdapterSetPlate1::MoveToHomePosition(const VisionPosition& target)
    {
        if (!m_parts) return ActFail;

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

        return ActOk;
    }

    ActError PLVIAdapterSetPlate1::IsAtHomePosition(const VisionPosition& target)
    {
        if (!m_parts) return ActFail;

        // Y축 도착 확인
        if (target.pos.size() > 1)
        {
            double yDiff = std::abs(m_parts->AxisY.GetEncoder() - target.pos[1]);
            if (yDiff > 1.0)
                return ActWait;
        }

        // Z축 도착 확인
        if (target.pos.size() > 2)
        {
            double zDiff = std::abs(m_parts->AxisZ.GetEncoder() - target.pos[2]);
            if (zDiff > 1.0)
                return ActWait;
        }

        // 실린더 리프트 업 확인
        if (m_parts->CylSetplateUpDown.size() > 0)
        {
            if (!m_parts->CylSetplateUpDown[0].isUp())
                return ActWait;
        }

        return ActOk;
    }

    // ── Z축 이동/확인 (VisionPosition 기반) ──
    ActError PLVIAdapterSetPlate1::MoveToZ(const VisionPosition& target)
    {
        return MoveZ(target);
    }

    ActError PLVIAdapterSetPlate1::IsAtZ(const VisionPosition& target)
    {
        return isMoveZ(target);
    }

    // ── 안전 Z 이동/확인 (VisionPosition 기반) ──
    ActError PLVIAdapterSetPlate1::MoveToZSafe(const VisionPosition& target)
    {
        if (!m_parts) return ActFail;

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

        return ActOk;
    }

    ActError PLVIAdapterSetPlate1::IsAtZSafe(const VisionPosition& target)
    {
        if (!m_parts) return ActFail;

        double safeZ = 0.0;
        if (target.pos.size() > 2)
            safeZ = target.pos[2];

        double zDiff = std::abs(m_parts->AxisZ.GetEncoder() - safeZ);
        if (zDiff > 1.0)
            return ActWait;

        // 실린더 리프트 업 확인
        if (m_parts->CylSetplateUpDown.size() > 0)
        {
            if (!m_parts->CylSetplateUpDown[0].isUp())
                return ActWait;
        }

        return ActOk;
    }

    // ── 조명/레이저 제어 ──
    int PLVIAdapterSetPlate1::SetLightState(int camIndex, bool on, int lightIndex)
    {
        (void)lightIndex;  // 현재는 camIndex만 사용
        if (!m_parts) return 0;
        switch (camIndex)
        {
        case 0: m_parts->LampPLVI.SetStatus(on); break;
        case 1: m_parts->LampSide.SetStatus(on); break;
        default: return 0;
        }
        return 1;
    }

    int PLVIAdapterSetPlate1::GetLightState(int camIndex, bool& outOn, int lightIndex)
    {
        (void)lightIndex;  // 현재는 camIndex만 사용
        if (!m_parts) return 0;
        switch (camIndex)
        {
        case 0: outOn = m_parts->LampPLVI.GetStatus(); break;
        case 1: outOn = m_parts->LampSide.GetStatus(); break;
        default: return 0;
        }
        return 1;
    }

    // ── 트리거 제어 ──
    // Task는 enable/intervalMm만 전달
    // scanDistMm, accelDistMm은 이 Adapter가 위치에 맞게 보유
    //
    // 실제 HW 연결 시
    //   g_Motor[eAXIS_PLVI_MOD_1]->SetPlviCFG_REG(
    //       enable,
    //       m_scanDistMm  * 1000.0,   // mm → um
    //       m_accelDistMm * 1000.0,   // mm → um
    //       intervalMm    * 1000.0    // mm → um
    //   );
    ActError PLVIAdapterSetPlate1::SetTriggerState(bool enable, double intervalMm, int triggerIndex)
    {
        (void)triggerIndex;  // 현재는 단일 트리거만 사용
        if (!m_parts) return ActFail;

        m_triggerEnabled = enable;
        m_triggerIntervalMm = intervalMm;

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

        return ActOk;
    }

    ActError PLVIAdapterSetPlate1::GetTriggerState(bool& outEnabled, double& outIntervalMm, int triggerIndex)
    {
        (void)triggerIndex;  // 현재는 단일 트리거만 사용
        outEnabled = m_triggerEnabled;
        outIntervalMm = m_triggerIntervalMm;
        return ActOk;
    }

    // ── 레이저 제어 ──
    ActError PLVIAdapterSetPlate1::SetLaserState(int laserChannel, bool on, int laserIndex)
    {
        (void)laserIndex;  // 현재는 laserChannel만 사용
        if (!m_parts) return ActFail;
        // TODO: 실제 레이저 제어 구현
        // m_parts->Laser.SetStatus(on);
        return ActOk;
    }

    ActError PLVIAdapterSetPlate1::GetLaserState(int laserChannel, bool& outOn, int laserIndex)
    {
        (void)laserIndex;  // 현재는 laserChannel만 사용
        if (!m_parts) return ActFail;
        // TODO: 실제 레이저 상태 조회 구현
        // outOn = m_parts->Laser.GetStatus();
        outOn = false;
        return ActOk;
    }

    // ── PLVI 전용 헬퍼 ──
    ActError PLVIAdapterSetPlate1::ClampSetplate(int idx, bool clamp)
    {
        if (!m_parts) return ActFail;
        if (idx < 0 || idx >= static_cast<int>(m_parts->CylSetplateClamp.size()))
            return ActInvalidParam;

        if (clamp)
            m_parts->CylSetplate
