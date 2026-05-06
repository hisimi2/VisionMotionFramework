#include "StdAfx.h"
#include "CLoad1PerformBacklashScanningTask.h"
#include "DVH_VAT/DefineVAT.h"
#include "VisionMemoryKeys.h"

using namespace VAT_LOAD1::Task;

CLoad1PerformBacklashScanningTask::CLoad1PerformBacklashScanningTask()
	: m_cameraId(0)
	, m_zeroPositionX(0.0)
	, m_zeroPositionY(0.0)
	, m_startPositionX(0.0)
	, m_startPositionY(0.0)
	, m_endPositionX(0.0)
	, m_endPositionY(0.0)
	, m_currentPositionX(0.0)
	, m_currentPositionY(0.0)
	, m_targetTablePosition(-65.0)
	, m_startOffsetX(0.0)
	, m_startOffsetY(0.0)
	, m_endOffsetX(0.0)
	, m_endOffsetY(0.0)
	, m_currentSpeedIndex(0)
	, m_focusPositionZ(0.0)
	, m_isStartMeasured(false)
	, m_visionTimeoutMs(30000)
	, m_moveTimeoutMs(7000)
	, m_nextStateAfterSafeZ(MoveOrigin)
	, m_scanAxis(Axis_Y)
{
}

CLoad1PerformBacklashScanningTask::~CLoad1PerformBacklashScanningTask()
{
}

void CLoad1PerformBacklashScanningTask::OnInitialize(DVH_VAT::VAT_Context& ctx)
{
	m_cameraId = ctx.GetSeqParamAs<int>(VAT_SEQ_PARAM_CAM_INDEX, 0);
	m_zeroPositionX = ctx.GetSeqParamAs<double>(VAT_SEQ_PARAM_ZERO_POS_X, 0.0);
	m_zeroPositionY = ctx.GetSeqParamAs<double>(VAT_SEQ_PARAM_ZERO_POS_Y, 0.0);
	m_startPositionX = ctx.GetSeqParamAs<double>(VAT_SEQ_PARAM_START_POS_X, 0.0);
	m_startPositionY = ctx.GetSeqParamAs<double>(VAT_SEQ_PARAM_START_POS_Y, 0.0);
	m_endPositionX = ctx.GetSeqParamAs<double>(VAT_SEQ_PARAM_END_POS_X, 0.0);
	m_endPositionY = ctx.GetSeqParamAs<double>(VAT_SEQ_PARAM_END_POS_Y, 0.0);

	DVH_VAT::VisionPosition position;
	ctx.PeekVisionPosition(position);
	m_focusPositionZ = position.pos[2];

	const std::string axisStr = ctx.GetSeqParamAs<std::string>(VAT_SEQ_PARAM_SCAN_AXIS, std::string());
	if (!axisStr.empty())
	{
		if (axisStr == "X" || axisStr == "x")
			m_scanAxis = Axis_X;
		else
			m_scanAxis = Axis_Y;
	}

	m_speedList.clear();

	SpeedInfo speedInfo;
	speedInfo.speed = 10000;
	speedInfo.acc = 50;
	speedInfo.dcc = 50;
	m_speedList.push_back(speedInfo);

	speedInfo.speed = 20000;
	speedInfo.acc = 40;
	speedInfo.dcc = 40;
	m_speedList.push_back(speedInfo);

	speedInfo.speed = 30000;
	speedInfo.acc = 40;
	speedInfo.dcc = 40;
	m_speedList.push_back(speedInfo);

	m_currentSpeedIndex = 0;
	m_backlashResults.clear();
	m_isStartMeasured = false;
	m_nextStateAfterSafeZ = MoveOrigin;

	EnterState(MoveSafeZ);
}

void CLoad1PerformBacklashScanningTask::ApplyCurrentSpeed(DVH_VAT::IVatActuator* actuator)
{
	if (!actuator)
	{
		return;
	}

	if (m_currentSpeedIndex < 0 || static_cast<size_t>(m_currentSpeedIndex) >= m_speedList.size())
	{
		return;
	}

	const SpeedInfo& speedInfo = m_speedList[m_currentSpeedIndex];
	(void)speedInfo;
}

DVH_VAT::TaskResult CLoad1PerformBacklashScanningTask::OnPoll(
	DVH_VAT::VAT_Context& ctx,
	DVH_VAT::IVatActuator* actuator)
{
	switch (GetState())
	{
	case MoveSafeZ:          return HandleMoveSafeZ(ctx, actuator);
	case MoveOrigin:         return HandleMoveOrigin(ctx, actuator);
	case MoveStart:          return HandleMoveStart(ctx, actuator);
	case MoveZero:           return HandleMoveZero(ctx, actuator);
	case MoveEnd:            return HandleMoveEnd(ctx, actuator);
	case MoveFocusZ:         return HandleMoveFocusZ(ctx, actuator);
	case VisionRequest:      return HandleVisionRequest(ctx, actuator);
	case VisionWait:         return HandleVisionWait(ctx, actuator);
	case CalculateBacklash:  return HandleCalculateBacklash(ctx, actuator);
	case SaveBacklashResult: return HandleSaveBacklashResult(ctx);
	default:                 return DVH_VAT::TR_ERROR;
	}
}

DVH_VAT::TaskResult CLoad1PerformBacklashScanningTask::HandleMoveSafeZ(
	DVH_VAT::VAT_Context& ctx,
	DVH_VAT::IVatActuator* actuator)
{
	if (!actuator)
		return SetErrorAndReturn(ctx, "Actuator null");

	if (actuator->MoveZ(0.0) != DVH_VAT::ActOk)
		return SetErrorAndReturn(ctx, "MoveZ failed");

	ApplyCurrentSpeed(actuator);

	EnterStateWithTimeout(m_nextStateAfterSafeZ, m_moveTimeoutMs);
	return DVH_VAT::TR_KEEP;
}

DVH_VAT::TaskResult CLoad1PerformBacklashScanningTask::HandleMoveOrigin(
	DVH_VAT::VAT_Context& ctx,
	DVH_VAT::IVatActuator* actuator)
{
	if (actuator->isMoveZ(0.0) != DVH_VAT::ActOk)
	{
		if (IsDeadlineExpired())
			return SetErrorAndReturn(ctx, "Backlash: Move Z timeout.");

		return DVH_VAT::TR_KEEP;
	}

	std::vector<double> nextPosition;
	nextPosition.push_back(0.0);
	nextPosition.push_back(0.0);
	nextPosition.push_back(0.0);
	nextPosition.push_back(m_targetTablePosition);
	nextPosition.push_back(m_targetTablePosition);

	if (actuator->Move(nextPosition, DVH_VAT::Narrow) != DVH_VAT::ActOk)
		return SetErrorAndReturn(ctx, "Move Origin failed");

	m_currentPositionX = 0.0;
	m_currentPositionY = 0.0;

	EnterStateWithTimeout(MoveStart, m_moveTimeoutMs);
	return DVH_VAT::TR_KEEP;
}

DVH_VAT::TaskResult CLoad1PerformBacklashScanningTask::HandleMoveStart(
	DVH_VAT::VAT_Context& ctx,
	DVH_VAT::IVatActuator* actuator)
{
	std::vector<double> currentPosition;
	currentPosition.push_back(m_currentPositionX);
	currentPosition.push_back(m_currentPositionY);
	currentPosition.push_back(0.0);
	currentPosition.push_back(m_targetTablePosition);
	currentPosition.push_back(m_targetTablePosition);

	if (actuator->isMove(currentPosition, DVH_VAT::Narrow) != DVH_VAT::ActOk)
	{
		if (IsDeadlineExpired())
			return SetErrorAndReturn(ctx, "Backlash: Move Origin timeout.");

		return DVH_VAT::TR_KEEP;
	}

	m_currentPositionX = m_startPositionX;
	m_currentPositionY = m_startPositionY;

	std::vector<double> nextPosition;
	nextPosition.push_back(m_currentPositionX);
	nextPosition.push_back(m_currentPositionY);
	nextPosition.push_back(0.0);
	nextPosition.push_back(m_targetTablePosition);
	nextPosition.push_back(m_targetTablePosition);

	if (actuator->Move(nextPosition, DVH_VAT::Narrow) != DVH_VAT::ActOk)
		return SetErrorAndReturn(ctx, "MoveZero failed");

	m_nextStateAfterSafeZ = MoveStart;

	if (m_isStartMeasured)
	{
		EnterStateWithTimeout(MoveEnd, m_moveTimeoutMs);
	}
	else
	{
		EnterStateWithTimeout(MoveZero, m_moveTimeoutMs);
	}

	return DVH_VAT::TR_KEEP;
}

DVH_VAT::TaskResult CLoad1PerformBacklashScanningTask::HandleMoveZero(
	DVH_VAT::VAT_Context& ctx,
	DVH_VAT::IVatActuator* actuator)
{
	std::vector<double> currentPosition;
	currentPosition.push_back(m_currentPositionX);
	currentPosition.push_back(m_currentPositionY);
	currentPosition.push_back(0.0);
	currentPosition.push_back(m_targetTablePosition);
	currentPosition.push_back(m_targetTablePosition);

	if (actuator->isMove(currentPosition, DVH_VAT::Narrow) != DVH_VAT::ActOk)
	{
		if (IsDeadlineExpired())
			return SetErrorAndReturn(ctx, "Backlash: Move Zero timeout.");

		return DVH_VAT::TR_KEEP;
	}

	m_currentPositionX = m_zeroPositionX;
	m_currentPositionY = m_zeroPositionY;

	std::vector<double> nextPosition;
	nextPosition.push_back(m_currentPositionX);
	nextPosition.push_back(m_currentPositionY);
	nextPosition.push_back(0.0);
	nextPosition.push_back(m_targetTablePosition);
	nextPosition.push_back(m_targetTablePosition);

	if (actuator->Move(nextPosition, DVH_VAT::Narrow) != DVH_VAT::ActOk)
		return SetErrorAndReturn(ctx, "Move Start failed");

	EnterStateWithTimeout(MoveFocusZ, m_moveTimeoutMs);
	return DVH_VAT::TR_KEEP;
}

DVH_VAT::TaskResult CLoad1PerformBacklashScanningTask::HandleMoveEnd(
	DVH_VAT::VAT_Context& ctx,
	DVH_VAT::IVatActuator* actuator)
{
	std::vector<double> currentPosition;
	currentPosition.push_back(m_currentPositionX);
	currentPosition.push_back(m_currentPositionY);
	currentPosition.push_back(0.0);
	currentPosition.push_back(m_targetTablePosition);
	currentPosition.push_back(m_targetTablePosition);

	if (actuator->isMove(currentPosition, DVH_VAT::Narrow) != DVH_VAT::ActOk)
	{
		if (IsDeadlineExpired())
			return SetErrorAndReturn(ctx, "Backlash: Move Start timeout.");

		return DVH_VAT::TR_KEEP;
	}

	m_currentPositionX = m_endPositionX;
	m_currentPositionY = m_endPositionY;

	std::vector<double> nextPosition;
	nextPosition.push_back(m_currentPositionX);
	nextPosition.push_back(m_currentPositionY);
	nextPosition.push_back(0.0);
	nextPosition.push_back(m_targetTablePosition);
	nextPosition.push_back(m_targetTablePosition);

	if (actuator->Move(nextPosition, DVH_VAT::Narrow) != DVH_VAT::ActOk)
		return SetErrorAndReturn(ctx, "Move End failed");

	EnterStateWithTimeout(MoveZero, m_moveTimeoutMs);
	return DVH_VAT::TR_KEEP;
}

DVH_VAT::TaskResult CLoad1PerformBacklashScanningTask::HandleMoveFocusZ(
	DVH_VAT::VAT_Context& ctx,
	DVH_VAT::IVatActuator* actuator)
{
	if (!actuator)
		return SetErrorAndReturn(ctx, "Actuator null");

	std::vector<double> currentPosition;
	currentPosition.push_back(m_currentPositionX);
	currentPosition.push_back(m_currentPositionY);
	currentPosition.push_back(0.0);
	currentPosition.push_back(m_targetTablePosition);
	currentPosition.push_back(m_targetTablePosition);

	if (actuator->isMove(currentPosition, DVH_VAT::Narrow) != DVH_VAT::ActOk)
	{
		if (IsDeadlineExpired())
			return SetErrorAndReturn(ctx, "XY Timeout");

		return DVH_VAT::TR_KEEP;
	}

	if (actuator->MoveZ(m_focusPositionZ) != DVH_VAT::ActOk)
		return SetErrorAndReturn(ctx, "MoveZ failed");

	EnterStateWithTimeout(VisionRequest, m_moveTimeoutMs);
	return DVH_VAT::TR_KEEP;
}

DVH_VAT::TaskResult CLoad1PerformBacklashScanningTask::HandleVisionRequest(
	DVH_VAT::VAT_Context& ctx,
	DVH_VAT::IVatActuator* actuator)
{
	if (actuator->isMoveZ(m_focusPositionZ) != DVH_VAT::ActOk)
	{
		if (IsDeadlineExpired())
			return SetErrorAndReturn(ctx, "PerformPCDScanning: Z Down Fail");
	}

	if (!actuator)
		return SetErrorAndReturn(ctx, "PerformPCDScanning: light null");

	actuator->SetLightState(m_cameraId, true);

	auto visionProcessor = ctx.GetVisionProcessorInterface();
	visionProcessor->InitializeRecvThread();

	if (!ctx.ExecuteVisionCommand(DVH_VAT::Measure))
	{
		return SetErrorAndReturn(ctx, "Vision Command Failed");
	}

	EnterStateWithTimeout(VisionWait, m_visionTimeoutMs);
	return DVH_VAT::TR_KEEP;
}

DVH_VAT::TaskResult CLoad1PerformBacklashScanningTask::HandleVisionWait(
	DVH_VAT::VAT_Context& ctx,
	DVH_VAT::IVatActuator* actuator)
{
	if (IsDeadlineExpired())
	{
		return SetErrorAndReturn(ctx, "Vision Timeout");
	}

	auto visionProcessor = ctx.GetVisionProcessorInterface();

	if (!visionProcessor->IsValid(DVH_VAT::Measure))
		return DVH_VAT::TR_KEEP;

	auto data = visionProcessor->GetLatestData(DVH_VAT::Measure);

	double offsetX = 0.0;
	double offsetY = 0.0;

	auto itXOffset = data.find(VAT_VISION_KEY_X_OFFSET);
	if (itXOffset != data.end())
		offsetX = std::stod(itXOffset->second);

	auto itYOffset = data.find(VAT_VISION_KEY_Y_OFFSET);
	if (itYOffset != data.end())
		offsetY = std::stod(itYOffset->second);

	actuator->SetLightState(m_cameraId, false);

	if (!m_isStartMeasured)
	{
		m_isStartMeasured = true;
		m_startOffsetX = offsetX;
		m_startOffsetY = offsetY;
		EnterStateWithTimeout(m_nextStateAfterSafeZ, m_moveTimeoutMs);
	}
	else
	{
		m_endOffsetX = offsetX;
		m_endOffsetY = offsetY;
		EnterState(CalculateBacklash);
	}

	return DVH_VAT::TR_KEEP;
}

DVH_VAT::TaskResult CLoad1PerformBacklashScanningTask::HandleCalculateBacklash(
	DVH_VAT::VAT_Context& ctx,
	DVH_VAT::IVatActuator* actuator)
{
	double currentResult = 0.0;

	switch (m_scanAxis)
	{
	case Axis_X:
		currentResult = m_startOffsetX - m_endOffsetX;
		break;

	case Axis_Y:
		currentResult = m_startOffsetY - m_endOffsetY;
		break;
	}

	m_backlashResults.push_back(currentResult);
	++m_currentSpeedIndex;

	if (static_cast<size_t>(m_currentSpeedIndex) < m_speedList.size())
	{
		m_nextStateAfterSafeZ = MoveOrigin;
		m_isStartMeasured = false;
		ApplyCurrentSpeed(actuator);
		EnterStateWithTimeout(MoveSafeZ, m_moveTimeoutMs);
	}
	else
	{
		EnterState(SaveBacklashResult);
	}

	return DVH_VAT::TR_KEEP;
}

DVH_VAT::TaskResult CLoad1PerformBacklashScanningTask::HandleSaveBacklashResult(DVH_VAT::VAT_Context& ctx)
{
	for (size_t i = 0; i < m_speedList.size(); ++i)
	{
		(void)i;
	}

	return DVH_VAT::TR_NEXT;
}
