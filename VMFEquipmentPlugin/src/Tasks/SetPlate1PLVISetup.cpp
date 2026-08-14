#include "pch.h"
#include "SetPlate1PLVISetup.h"
#include "IParamProvider.h"
#include "ParamKeys.h"

using namespace VMF;
using namespace VMF_PLUGIN;

SetPlate1PLVISetup::SetPlate1PLVISetup()
    : m_scanStartX(0.0), m_scanStartY(0.0), m_scanStartZ(0.0)
    , m_triggerIntervalMm(1.8)
    , m_moveTimeoutMs(7000)
{
}

SetPlate1PLVISetup::~SetPlate1PLVISetup() {}

void SetPlate1PLVISetup::OnInitialize(VMF::Context& ctx)
{
    // IParamProvider 인터페이스를 통해 실행 파라미터를 조회
    const IParamProvider& provider = static_cast<const IParamProvider&>(ctx);
    m_moveTimeoutMs = provider.GetTaskParams().GetExecutionParam<int>(ParamKeys::Setup::TIMEOUT_MOVE_MS, 7000);
    m_triggerIntervalMm = provider.GetTaskParams().GetExecutionParam<double>(ParamKeys::Setup::TRIGGER_INTERVAL_MM, 1.8);

    // VisionPositions에서 시작 위치 조회
    std::vector<VMF::VisionPosition> positions = provider.GetVisionPositions();
    if (!positions.empty())
    {
        m_scanStartX = positions[0].pos.size() > 0 ? positions[0].pos[0] : 0.0;
        m_scanStartY = positions[0].pos.size() > 1 ? positions[0].pos[1] : 0.0;
        m_scanStartZ = positions[0].pos.size() > 2 ? positions[0].pos[2] : 0.0;
    }

    EnterState(MoveSafeZ);
}

VMF::TaskResult SetPlate1PLVISetup::OnPoll(VMF::Context& ctx, VMF::IActuator* actuator)
{
    switch (GetState())
    {
    case MoveSafeZ:        return HandleMoveSafeZ(ctx, actuator);
    case WaitSafeZ:        return HandleWaitSafeZ(ctx, actuator);
    case MoveHorizontalStart:  return HandleMoveHorizontalStart(ctx, actuator);
    case WaitHorizontalStart:  return HandleWaitHorizontalStart(ctx, actuator);
    case SetupTrigger:     return HandleSetupTrigger(ctx, actuator);
    case Complete:         return HandleComplete(ctx, actuator);
    default:               return TR_ERROR;
    }
}

// Z축 안전 위치로 이동
VMF::TaskResult SetPlate1PLVISetup::HandleMoveSafeZ(VMF::Context& ctx, VMF::IActuator* actuator)
{
    if (!actuator)
        return SetErrorAndReturn(ctx, "PLVI_Setup: actuator is null.");

    if (actuator->MoveToZ(0.0) != ActError::ActOk)
        return SetErrorAndReturn(ctx, "PLVI_Setup: MoveToZ safe failed.");

    EnterStateWithTimeout(WaitSafeZ, m_moveTimeoutMs);
    return TR_KEEP;
}

// Z축 안전 위치 도착 확인
VMF::TaskResult SetPlate1PLVISetup::HandleWaitSafeZ(VMF::Context& ctx, VMF::IActuator* actuator)
{
    if (!actuator)
        return SetErrorAndReturn(ctx, "PLVI_Setup: actuator is null.");

    if (actuator->IsAtZ(0.0) != ActError::ActOk)
    {
        if (IsDeadlineExpired())
            return SetErrorAndReturn(ctx, "PLVI_Setup: SafeZ timeout.");
        return TR_KEEP;
    }

    EnterStateWithTimeout(MoveHorizontalStart, m_moveTimeoutMs);
    return TR_KEEP;
}

// 수평 시작 위치로 이동
VMF::TaskResult SetPlate1PLVISetup::HandleMoveHorizontalStart(VMF::Context& ctx, VMF::IActuator* actuator)
{
    if (!actuator)
        return SetErrorAndReturn(ctx, "PLVI_Setup: actuator is null.");

    VisionPosition targetPos;
    targetPos.pos = { m_scanStartX, m_scanStartY, 0.0 };

    if (actuator->MoveToMeasurementPosition(targetPos) != ActError::ActOk)
        return SetErrorAndReturn(ctx, "PLVI_Setup: MoveToMeasurementPosition failed.");

    EnterStateWithTimeout(WaitHorizontalStart, m_moveTimeoutMs);
    return TR_KEEP;
}

// 수평 시작 위치 도착 확인
VMF::TaskResult SetPlate1PLVISetup::HandleWaitHorizontalStart(VMF::Context& ctx, VMF::IActuator* actuator)
{
    if (!actuator)
        return SetErrorAndReturn(ctx, "PLVI_Setup: actuator is null.");

    VisionPosition targetPos;
    targetPos.pos = { m_scanStartX, m_scanStartY, 0.0 };

    if (actuator->IsAtMeasurementPosition(targetPos) != ActError::ActOk)
    {
        if (IsDeadlineExpired())
            return SetErrorAndReturn(ctx, "PLVI_Setup: MoveXY timeout.");
        return TR_KEEP;
    }

    EnterState(SetupTrigger);
    return TR_KEEP;
}

// 트리거 신호 발생 설정
VMF::TaskResult SetPlate1PLVISetup::HandleSetupTrigger(VMF::Context& ctx, VMF::IActuator* actuator)
{
    if (!actuator)
        return SetErrorAndReturn(ctx, "PLVI_Setup: actuator is null.");

    if (actuator->SetLaserState(0, true) != ActError::ActOk)
        return SetErrorAndReturn(ctx, "PLVI_Setup: SetLaserState failed.");

    if (actuator->SetTriggerState(true, m_triggerIntervalMm) != ActError::ActOk)
        return SetErrorAndReturn(ctx, "PLVI_Setup: SetTriggerState failed.");

    EnterState(Complete);
    return TR_KEEP;
}

// 완료
VMF::TaskResult SetPlate1PLVISetup::HandleComplete(VMF::Context& ctx, VMF::IActuator* actuator)
{
    (void)ctx;
    (void)actuator;
    return TR_NEXT;
}
