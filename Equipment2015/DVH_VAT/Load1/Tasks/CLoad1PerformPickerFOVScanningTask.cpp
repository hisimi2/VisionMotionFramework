#include "stdafx.h"
#include "CLoad1PerformPickerFOVScanningTask.h"
#include "DVH_VAT/DefineVAT.h"
#include "VisionMemoryKeys.h"

using namespace VAT_LOAD1::Task;

CLoad1PerformPickerFOVScanningTask::CLoad1PerformPickerFOVScanningTask()
    : m_currentScanDirection(CENTER)
    , m_centerPositionX(0.0)
    , m_centerPositionY(0.0)
    , m_focusPositionZ(0.0)
    , m_fovIntervalX(0.0)
    , m_fovIntervalY(0.0)
    , m_centerOffsetX(0.0)
    , m_centerOffsetY(0.0)
    , m_frontRightOffsetX(0.0)
    , m_frontRightOffsetY(0.0)
    , m_cameraId(0)
    , m_packageId(0)
    , m_visionTimeoutMs(30000)
    , m_moveTimeoutMs(7000)
{
}

CLoad1PerformPickerFOVScanningTask::~CLoad1PerformPickerFOVScanningTask()
{
}

void CLoad1PerformPickerFOVScanningTask::OnInitialize(VMF::VAT_Context& ctx)
{
    m_cameraId              = ctx.GetSeqParamAs<int>(VAT_SEQ_PARAM_CAMERA_INDEX,                0);
    m_packageId             = ctx.GetSeqParamAs<int>(VAT_SEQ_PARAM_PACKAGE_ID,                  0);
    m_moveTimeoutMs         = ctx.GetSeqParamAs<int>(VAT_SEQ_PARAM_MOTION_TIMEOUT_MS,           0);
    m_visionTimeoutMs       = ctx.GetSeqParamAs<int>(VAT_SEQ_PARAM_VISION_TIMEOUT_MS,           0);
    m_fovIntervalX          = ctx.GetSeqParamAs<double>(VAT_SEQ_PARAM_PICKER_FOV_INTERVAL_X,    2.0);
    m_fovIntervalY          = ctx.GetSeqParamAs<double>(VAT_SEQ_PARAM_PICKER_FOV_INTERVAL_Y,    2.0);
    m_currentScanDirection  = CENTER;
    m_centerOffsetX         = 0.0;
    m_centerOffsetY         = 0.0;
    m_frontRightOffsetX     = 0.0;
    m_frontRightOffsetY     = 0.0;

    // 초기화 시점에 현재 위치를 읽어서 CENTER 스캔의 기준으로 사용
    EnterState(MoveSafeZ);
}

VMF::TaskResult CLoad1PerformPickerFOVScanningTask::OnPoll(
    VMF::VAT_Context& ctx,
    VMF::IVatActuator* actuator)
{
    switch (GetState())
    {
    case MoveSafeZ:
        return HandleMoveSafeZ(ctx, actuator);

    case MoveOrigin:
        return HandleMoveOrigin(ctx, actuator);

    case MoveScanPosition:
        return HandleMoveScanPosition(ctx, actuator);

    case MoveFocusPositionZ:
        return HandleMoveFocusPositionZ(ctx, actuator);

    case VisionRequest:
        return HandleVisionRequest(ctx, actuator);

    case VisionWait:
        return HandleVisionWait(ctx, actuator);

    case SaveResult:
        return HandleSaveResult(ctx);

    case ReturnHome:
        return HandleReturnHome(ctx, actuator);

    default:
        return SetErrorAndReturn(ctx, "Unknown state in CLoad1PerformPickerFOVScanningTask");
    }
}

VMF::TaskResult CLoad1PerformPickerFOVScanningTask::HandleMoveSafeZ(
    VMF::VAT_Context& ctx,
    VMF::IVatActuator* actuator)
{
    if (!actuator)
    {
        return SetErrorAndReturn(ctx, "PickerFOV: actuator is null.");
    }

    if (m_currentScanDirection == CENTER)
    {
        std::vector<double> currentPosition = actuator->getPosition();

        m_centerPositionX   = currentPosition[0];
        m_centerPositionY   = currentPosition[1];
        m_focusPositionZ    = currentPosition[2];
    }

    if (actuator->MoveZ(0.0) != VMF::ActOk)
    {
        return SetErrorAndReturn(ctx, "PickerFOV: MoveToSafeZ failed.");
    }

    EnterStateWithTimeout(MoveOrigin, m_moveTimeoutMs);
    return VMF::TR_KEEP;
}

VMF::TaskResult CLoad1PerformPickerFOVScanningTask::HandleMoveOrigin(
    VMF::VAT_Context& ctx,
    VMF::IVatActuator* actuator)
{
    if (!actuator)
    {
        return SetErrorAndReturn(ctx, "PickerFOV: actuator is null.");
    }

    if (actuator->isMoveZ(0.0) != VMF::ActOk)
    {
        if (IsDeadlineExpired())
        {
            return SetErrorAndReturn(ctx, "PickerFOV: SafeZ timeout.");
        }

        return VMF::TR_KEEP;
    }

    std::vector<double> originXY;
    originXY.push_back(0.0);
    originXY.push_back(0.0);

    if (actuator->Move(originXY, VMF::Narrow) != VMF::ActOk)
    {
        return SetErrorAndReturn(ctx, "PickerFOV: MoveToOrigin failed.");
    }

    EnterStateWithTimeout(MoveScanPosition, m_moveTimeoutMs);
    return VMF::TR_KEEP;
}

VMF::TaskResult CLoad1PerformPickerFOVScanningTask::HandleMoveScanPosition(
    VMF::VAT_Context& ctx,
    VMF::IVatActuator* actuator)
{
    if (!actuator)
    {
        return SetErrorAndReturn(ctx, "PickerFOV: actuator is null.");
    }

    std::vector<double> originXY;
    originXY.push_back(0.0);
    originXY.push_back(0.0);

    if (actuator->isMove(originXY, VMF::Narrow) != VMF::ActOk)
    {
        if (IsDeadlineExpired())
        {
            return SetErrorAndReturn(ctx, "PickerFOV: Origin XY timeout.");
        }

        return VMF::TR_KEEP;
    }

    double targetX = m_centerPositionX;
    double targetY = m_centerPositionY;

    switch (m_currentScanDirection)
    {
    case CENTER:
        break;

    case FRONT_RIGHT:
        targetX = m_centerPositionX + m_fovIntervalX;
        targetY = m_centerPositionY - m_fovIntervalY;
        break;

    default:
        EnterStateWithTimeout(ReturnHome, m_moveTimeoutMs);
        return VMF::TR_KEEP;
    }

    std::vector<double> targetXY;
    targetXY.push_back(targetX);
    targetXY.push_back(targetY);

    if (actuator->Move(targetXY, VMF::Narrow) != VMF::ActOk)
    {
        return SetErrorAndReturn(ctx, "PickerFOV: XY Move failed.");
    }

    EnterStateWithTimeout(MoveFocusPositionZ, m_moveTimeoutMs);
    return VMF::TR_KEEP;
}

VMF::TaskResult CLoad1PerformPickerFOVScanningTask::HandleMoveFocusPositionZ(
    VMF::VAT_Context& ctx,
    VMF::IVatActuator* actuator)
{
    if (!actuator)
    {
        return SetErrorAndReturn(ctx, "PickerFOV: actuator is null.");
    }

    // target은 MoveScanPosition과 동일한 계산으로 재생성
    double targetX = m_centerPositionX;
    double targetY = m_centerPositionY;

    switch (m_currentScanDirection)
    {
    case CENTER:
        break;

    case FRONT_RIGHT:
        targetX = m_centerPositionX + m_fovIntervalX;
        targetY = m_centerPositionY - m_fovIntervalY;
        break;

    default:
        EnterStateWithTimeout(ReturnHome, m_moveTimeoutMs);
        return VMF::TR_KEEP;
    }

    std::vector<double> targetXY;
    targetXY.push_back(targetX);
    targetXY.push_back(targetY);

    if (actuator->isMove(targetXY, VMF::Narrow) != VMF::ActOk)
    {
        if (IsDeadlineExpired())
        {
            return SetErrorAndReturn(ctx, "PickerFOV: Target XY timeout.");
        }

        return VMF::TR_KEEP;
    }

    if (actuator->MoveZ(m_focusPositionZ) != VMF::ActOk)
    {
        return SetErrorAndReturn(ctx, "PickerFOV: MoveToFocusZ failed.");
    }

    EnterState(VisionRequest);
    return VMF::TR_KEEP;
}

VMF::TaskResult CLoad1PerformPickerFOVScanningTask::HandleVisionRequest(
    VMF::VAT_Context& ctx,
    VMF::IVatActuator* actuator)
{
    if (!actuator)
    {
        return SetErrorAndReturn(ctx, "PickerFOV: actuator is null.");
    }

    if (actuator->isMoveZ(m_focusPositionZ) != VMF::ActOk)
    {
        if (IsDeadlineExpired())
        {
            return SetErrorAndReturn(ctx, "PickerFOV: Focus Z timeout.");
        }

        return VMF::TR_KEEP;
    }

    actuator->SetLightState(m_cameraId, true);

    auto visionProcessor = ctx.GetVisionProcessorInterface();
    if (!visionProcessor)
    {
        return SetErrorAndReturn(ctx, "PickerFOV: No Vision Processor");
    }

    visionProcessor->InitializeRecvThread();

    const int visionRequestId = ctx.GetSeqParamAs<int>(VAT_SEQ_PARAM_VISION_PICKER_FOV_REQUEST_ID, 0);
    ctx.SetSeqParam(VAT_SEQ_PARAM_STATUS, visionRequestId);

    if (!ctx.ExecuteVisionCommand(VMF::Measure))
    {
        return SetErrorAndReturn(ctx, "PickerFOV: Vision Command Failed");
    }

    EnterStateWithTimeout(VisionWait, m_visionTimeoutMs);
    return VMF::TR_KEEP;
}

VMF::TaskResult CLoad1PerformPickerFOVScanningTask::HandleVisionWait(
    VMF::VAT_Context& ctx,
    VMF::IVatActuator* actuator)
{
    if (IsDeadlineExpired())
    {
        return SetErrorAndReturn(ctx, "PickerFOV: Vision Timeout");
    }

    auto visionProcessor = ctx.GetVisionProcessorInterface();
    if (!visionProcessor)
    {
        return SetErrorAndReturn(ctx, "PickerFOV: No Vision Processor");
    }

    if (!visionProcessor->IsValid(VMF::Measure))
    {
        return VMF::TR_KEEP;
    }

    auto data = visionProcessor->GetLatestData(VMF::Measure);

    double offsetX = 0.0;
    double offsetY = 0.0;

    auto itXOffset = data.find(VMF::X_OFFSET);
    if (itXOffset != data.end())
    {
        offsetX = std::stod(itXOffset->second);
    }

    auto itYOffset = data.find(VMF::Y_OFFSET);
    if (itYOffset != data.end())
    {
        offsetY = std::stod(itYOffset->second);
    }

    if (actuator)
    {
        actuator->SetLightState(m_cameraId, false);
    }

    if (m_currentScanDirection == CENTER)
    {
        m_centerOffsetX = offsetX;
        m_centerOffsetY = offsetY;

        m_currentScanDirection = FRONT_RIGHT;
        EnterState(MoveSafeZ);
        return VMF::TR_KEEP;
    }

    m_frontRightOffsetX = offsetX;
    m_frontRightOffsetY = offsetY;

    // 델타는 저장 시점에 계산하도록 변경 (멤버 제거)
    EnterState(SaveResult);
    return VMF::TR_KEEP;
}

VMF::TaskResult CLoad1PerformPickerFOVScanningTask::HandleSaveResult(
    VMF::VAT_Context& ctx)
{
    auto repo = ctx.getRepository();
    if (!repo)
    {
        return SetErrorAndReturn(ctx, "PickerFOV: Repository is null.");
    }

    ctx.SetSeqParamAs<double>("PickerFOVCenterOffsetX", m_centerOffsetX);
    ctx.SetSeqParamAs<double>("PickerFOVCenterOffsetY", m_centerOffsetY);
    ctx.SetSeqParamAs<double>("PickerFOVFrontRightOffsetX", m_frontRightOffsetX);
    ctx.SetSeqParamAs<double>("PickerFOVFrontRightOffsetY", m_frontRightOffsetY);

    // front-right 델타를 여기서 계산하여 저장
    const double frontRightDeltaX = m_frontRightOffsetX - m_centerOffsetX;
    const double frontRightDeltaY = m_frontRightOffsetY - m_centerOffsetY;
    ctx.SetSeqParamAs<double>("PickerFOVFrontRightDeltaX", frontRightDeltaX);
    ctx.SetSeqParamAs<double>("PickerFOVFrontRightDeltaY", frontRightDeltaY);

    EnterState(ReturnHome);
    return VMF::TR_KEEP;
}

VMF::TaskResult CLoad1PerformPickerFOVScanningTask::HandleReturnHome(
    VMF::VAT_Context& ctx,
    VMF::IVatActuator* actuator)
{
    if (!actuator)
    {
        return SetErrorAndReturn(ctx, "PickerFOV: actuator is null.");
    }

    if (actuator->MoveZ(0.0) != VMF::ActOk)
    {
        return SetErrorAndReturn(ctx, "PickerFOV: Z Home Return Fail");
    }

    EnterStateWithTimeout(ReturnHome, m_moveTimeoutMs);
    return VMF::TR_NEXT;
}
