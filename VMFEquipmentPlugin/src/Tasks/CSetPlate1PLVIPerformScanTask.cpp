#include "pch.h"
#include "CSetPlate1PLVIPerformScanTask.h"

// CMockPLVIEventHandler include 없음 - 순수 인터페이스(IVisionEventHandler)만 사용
// PLVI 구현체가 RequestMeasureAsync/GetLatestData(Measure) 등을
// PLVI 동작으로 오버라이딩했으므로 Task는 구현체를 알 필요가 없음

using namespace VMF;
using namespace VMF_PLUGIN;


CSetPlate1PLVIPerformScanTask::CSetPlate1PLVIPerformScanTask()
	: m_plviPosition(0)
	, m_ctrayX(8)
	, m_ctrayY(4)
	, m_dataId(1)
	, m_scanEndY(200.0)
	, m_scanSpeedMmS(100.0)
	, m_retryCount(0)
	, m_maxRetryCount(1)
	, m_timeoutMeasureMs(5000)
	, m_timeoutScanMs(15000)
	, m_timeoutResultMs(10000)
	, m_timeoutMoveMs(7000)
{}

CSetPlate1PLVIPerformScanTask::~CSetPlate1PLVIPerformScanTask() {}

void CSetPlate1PLVIPerformScanTask::OnInitialize(VMF::Context& ctx)
{
    //  T GetTaskSeqParamAs(Context& ctx, const std::string& key, const T& defaultValue) const
    m_plviPosition = GetTaskSeqParamAs<int>(ctx, "PLVI_POSITION");
    m_ctrayX = GetTaskSeqParamAs<int>(ctx, "CTRAY_X");
    m_ctrayY = GetTaskSeqParamAs<int>(ctx, "CTRAY_Y");
    m_dataId = GetTaskSeqParamAs<int>(ctx, "DATA_ID");
    m_scanSpeedMmS = GetTaskSeqParamAs<double>(ctx, "SCAN_SPEED_MM_S");
    m_maxRetryCount = GetTaskSeqParamAs<int>(ctx, "MAX_RETRY_COUNT");
    m_timeoutMeasureMs = GetTaskSeqParamAs<int>(ctx, "TIMEOUT_MEASURE_MS");
    m_timeoutScanMs = GetTaskSeqParamAs<int>(ctx, "TIMEOUT_SCAN_MS");
    m_timeoutResultMs = GetTaskSeqParamAs<int>(ctx, "TIMEOUT_RESULT_MS");
    m_timeoutMoveMs = GetTaskSeqParamAs<int>(ctx, "TIMEOUT_MOVE_MS");
    m_retryCount = 0;

    // Initialize vision positions using PeekTaskVisionPosition
    VisionPosition pos;
    if (PeekTaskVisionPosition(pos)) {
        m_scanStartPos = pos.pos;
    }

    EnterState(RequestMeasure);
}

VMF::TaskResult CSetPlate1PLVIPerformScanTask::OnPoll(VMF::Context& ctx, VMF::IActuator* actuator)
{
	switch (GetState())
	{
	case RequestMeasure:   return HandleRequestMeasure(ctx, actuator);
	case WaitMeasureAck:   return HandleWaitMeasureAck(ctx, actuator);
	case PerformScan:      return HandlePerformScan(ctx, actuator);
	case WaitScanComplete: return HandleWaitScanComplete(ctx, actuator);
	case RequestResult:    return HandleRequestResult(ctx, actuator);
	case WaitResult:       return HandleWaitResult(ctx, actuator);
	case SaveResult:       return HandleSaveResult(ctx, actuator);
	case CS_ERROR:
	default:               return TR_ERROR;
	}
}

// VisionOS에 PLVI 검사 요청
// CMockPLVIEventHandler::RequestMeasureAsync → SimulateMeasure → m_latestData[Measure] 저장
VMF::TaskResult CSetPlate1PLVIPerformScanTask::HandleRequestMeasure(VMF::Context& ctx, VMF::IActuator* actuator)
{
	auto vp = ctx.GetVisionProcessorInterface();
	if (!vp)
		return SetErrorAndReturn(ctx, "PLVI_PerformScan: No VisionProcessor.");

	vp->ClearLatestData(VMF::VisionCommand::Measure);

	if (!ctx.ExecuteVisionCommand(VMF::VisionCommand::Measure))
		return SetErrorAndReturn(ctx, "PLVI_PerformScan: Measure request failed.");

	EnterStateWithTimeout(WaitMeasureAck, m_timeoutMeasureMs);
	return TR_KEEP;
}

// 검사 시작 ACK 대기
// vp->HasReceived(Measure) / vp->GetLatestData(Measure) - 인터페이스 그대로
TaskResult CSetPlate1PLVIPerformScanTask::HandleWaitMeasureAck(Context& ctx, IActuator* actuator)
{
	auto vp = ctx.GetVisionProcessorInterface();
	if (!vp)
		return SetErrorAndReturn(ctx, "PLVI_PerformScan: No VisionProcessor.");

	if (!vp->HasReceived(Measure))
	{
		if (IsDeadlineExpired())
			return SetErrorAndReturn(ctx, "PLVI_PerformScan: MeasureAck timeout.");
		return TR_KEEP;
	}

	IVisionClient::DataMap ack = vp->GetLatestData(Measure);
	auto it = ack.find("STATUS");
	if (it == ack.end() || it->second != "1")
	{
		auto errIt = ack.find("ERR_CODE");
		std::string errMsg = "PLVI_PerformScan: Measure ACK failed.";
		if (errIt != ack.end())
			errMsg += " ErrCode=" + errIt->second;
		return SetErrorAndReturn(ctx, errMsg.c_str());
	}

	EnterStateWithTimeout(PerformScan, m_timeoutMoveMs);
	return TR_KEEP;
}

// Motor Y축 스캔
TaskResult CSetPlate1PLVIPerformScanTask::HandlePerformScan(Context& ctx, IActuator* actuator)
{
	if (!actuator)
		return SetErrorAndReturn(ctx, "PLVI_PerformScan: actuator is null.");

	// Y축 스캔 이동
	if (actuator->MoveToY(m_scanEndY) != ActError::ActOk)
		return SetErrorAndReturn(ctx, "PLVI_PerformScan: Scan move failed.");

	EnterStateWithTimeout(WaitScanComplete, m_timeoutScanMs);
	return TR_KEEP;
}

// 스캔 완료 확인, 트리거/Laser 해제
TaskResult CSetPlate1PLVIPerformScanTask::HandleWaitScanComplete(Context& ctx, IActuator* actuator)
{
	if (!actuator)
		return SetErrorAndReturn(ctx, "PLVI_PerformScan: actuator is null.");

	// Y축 스캔 완료 확인
	if (actuator->IsAtY(m_scanEndY) != ActError::ActOk)
	{
		if (IsDeadlineExpired())
			return SetErrorAndReturn(ctx, "PLVI_PerformScan: Scan timeout.");
		return TR_KEEP;
	}

	actuator->SetTriggerState(false, 0.0);
	actuator->SetLaserState(0, false);

	EnterStateWithTimeout(RequestResult, m_timeoutMeasureMs);
	return TR_KEEP;
}

// VisionOS에 결과 요청
// CMockPLVIEventHandler::RequestInspReadyAsync → SimulateGetResult → m_latestData[InspReady] 저장
TaskResult CSetPlate1PLVIPerformScanTask::HandleRequestResult(Context& ctx, IActuator* actuator)
{
	auto vp = ctx.GetVisionProcessorInterface();
	if (!vp)
		return SetErrorAndReturn(ctx, "PLVI_PerformScan: No VisionProcessor.");

	vp->ClearLatestData(InspReady);

	if (!ctx.ExecuteVisionCommand(InspReady))
		return SetErrorAndReturn(ctx, "PLVI_PerformScan: GetResult request failed.");

	EnterStateWithTimeout(WaitResult, m_timeoutResultMs);
	return TR_KEEP;
}

// 결과 수신 대기
// vp->IsValid(InspReady) - 인터페이스 그대로
TaskResult CSetPlate1PLVIPerformScanTask::HandleWaitResult(Context& ctx, IActuator* actuator)
{
	auto vp = ctx.GetVisionProcessorInterface();
	if (!vp)
		return SetErrorAndReturn(ctx, "PLVI_PerformScan: No VisionProcessor.");

	if (!vp->IsValid(InspReady))
	{
		if (IsDeadlineExpired())
			return SetErrorAndReturn(ctx, "PLVI_PerformScan: Result timeout.");
		return TR_KEEP;
	}

	EnterState(SaveResult);
	return TR_KEEP;
}

// 결과 파싱 + Retry + DB 저장
TaskResult CSetPlate1PLVIPerformScanTask::HandleSaveResult(Context& ctx, IActuator* actuator)
{
    // Use PopTaskVisionPosition instead of ctx.PopVisionPosition
    
    if (PopTaskVisionPosition(m_measurepos)) {
        m_retryCount = 0;
        EnterState(RequestMeasure);
        return TR_PREV;
    }

    // Rest of the code remains unchanged
    auto vp = ctx.GetVisionProcessorInterface();
    auto repo = ctx.GetRepository();
    if (!vp) return SetErrorAndReturn(ctx, "PLVI_PerformScan: No VisionProcessor.");
    if (!repo) return SetErrorAndReturn(ctx, "PLVI_PerformScan: No Repository.");

    IVisionClient::DataMap raw = vp->GetLatestData(InspReady);
    PLVIStatus result = ParsePLVIResult(raw);

    if (!result.resultOK) return SetErrorAndReturn(ctx, "PLVI_PerformScan: VisionOS returned error.");
    if (result.overallResult != 0) {
        if (m_retryCount < m_maxRetryCount) {
            ++m_retryCount;
            EnterState(RequestMeasure);
            return TR_PREV;
        }
        return SetErrorAndReturn(ctx, "PLVI_PerformScan: Result NG, retry exceeded.");
    }

    if (PopTaskVisionPosition(m_measurepos)) {
        m_retryCount = 0;
        EnterState(RequestMeasure);
        return TR_PREV;
    }

	return TR_NEXT;
}

// ParsePLVIResult - StringMap → PLVIStatus 2D 배열 원복
CSetPlate1PLVIPerformScanTask::PLVIStatus CSetPlate1PLVIPerformScanTask::ParsePLVIResult(
	const VMF::StringMap& data)
{
	PLVIStatus result;

	auto getStr = [&](const std::string& key, const std::string& def = "") -> std::string
	{
		auto it = data.find(key);
		return (it != data.end()) ? it->second : def;
	};
	auto getInt = [&](const std::string& key, int def = 0) -> int
	{
		auto it = data.find(key);
		return (it != data.end()) ? std::atoi(it->second.c_str()) : def;
	};

	result.resultOK = (getStr("STATUS") == "1");
	result.overallResult = getInt("OVERALL", 1);
	result.plviPosition = getInt("PLVI_POSITION");
	result.rows = getInt("ROWS");
	result.cols = getInt("COLS");
	result.normalCount = getInt("NORMAL_COUNT");
	result.emptyCount = getInt("EMPTY_COUNT");
	result.leaveCount = getInt("LEAVE_COUNT");
	result.doubleCount = getInt("DOUBLE_COUNT");
	result.missingCount = getInt("MISSING_COUNT");
	result.mismatchCount = getInt("MISMATCH_COUNT");

	result.pocketStatus.resize(result.rows, std::vector<int>(result.cols, -1));
	for (int r = 0; r < result.rows; ++r)
		for (int c = 0; c < result.cols; ++c)
		{
			std::ostringstream key;
			key << "POCKET_" << r << "_" << c;
			result.pocketStatus[r][c] = getInt(key.str(), -1);
		}

	return result;
}
