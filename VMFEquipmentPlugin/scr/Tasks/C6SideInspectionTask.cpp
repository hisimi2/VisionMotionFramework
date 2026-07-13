#include "pch.h"
#include "C6SideInspectionTask.h"
#include "Context.h"
#include "IActuator.h"

namespace VMF_6SIDE
{
	namespace Task
	{
		C6SideInspectionTask::C6SideInspectionTask()
			: m_facePosition(1)
			, m_selectCount(6)
			, m_cameraIndex(0)
			, m_timeoutMs(40000)
			, m_retryCount(0)
			, m_maxRetry(2)
		{}

		C6SideInspectionTask::~C6SideInspectionTask() {}

		void C6SideInspectionTask::OnInitialize(VMF::Context& ctx)
		{
			// Tag 파라미터에서 읽기
			m_facePosition = GetTaskSeqParamAs<int>(ctx, "FacePosition");
			m_selectCount = GetTaskSeqParamAs<int>(ctx, "SelectCount");
			m_cameraIndex = GetTaskSeqParamAs<int>(ctx, "CameraIndex");
			m_barcodeId = GetTaskSeqParamAs<std::string>(ctx, "BarcodeID");
			m_lotId = GetTaskSeqParamAs<std::string>(ctx, "LotID");
			m_retryCount = 0;

			EnterState(TurnOnLight);
		}

		VMF::TaskResult C6SideInspectionTask::OnPoll(
			VMF::Context& ctx, VMF::IActuator* actuator)
		{
			switch (GetState())
			{
			case TurnOnLight:     return HandleTurnOnLight(ctx, actuator);
			case SendInspRequest: return HandleSendInspRequest(ctx, actuator);
			case WaitInspResult:  return HandleWaitInspResult(ctx, actuator);
			case ProcessResult:   return HandleProcessResult(ctx, actuator);
			case TurnOffLight:    return HandleTurnOffLight(ctx, actuator);
			case Done:            return VMF::TR_NEXT;
			default:              return SetErrorAndReturn(ctx, "Unknown state");
			}
		}

		VMF::TaskResult C6SideInspectionTask::HandleTurnOnLight(
			VMF::Context& ctx, VMF::IActuator* actuator)
		{
			if (actuator) actuator->SetLightState(m_cameraIndex, true);
			EnterState(SendInspRequest);
			return VMF::TR_KEEP;
		}

		// ── 1102 H→V 요청 ────────────────────────────────────────
		VMF::TaskResult C6SideInspectionTask::HandleSendInspRequest(
			VMF::Context& ctx, VMF::IActuator* actuator)
		{
			auto vp = ctx.GetVisionProcessorInterface();
			if (!vp) return SetErrorAndReturn(ctx, "No VisionProcessor");

			vp->InitializeRecvThread();

			// 요청 파라미터 설정
			// VisionProcessor 내부에서 1102 패킷 구성 시 사용
			ctx.SetSeqParam("ReqFacePosition", m_facePosition);
			ctx.SetSeqParam("ReqSelectCount", m_selectCount);
			ctx.SetSeqParam("ReqSkip", 0);   // 0: Inspection

			if (!ctx.ExecuteVisionCommand(VMF::Measure))
				return SetErrorAndReturn(ctx, "1102 send failed");

			EnterStateWithTimeout(WaitInspResult, m_timeoutMs);
			return VMF::TR_KEEP;
		}

		// ── 1102 V→H 수신 대기 ──────────────────────────────────
		VMF::TaskResult C6SideInspectionTask::HandleWaitInspResult(
			VMF::Context& ctx, VMF::IActuator* actuator)
		{
			auto vp = ctx.GetVisionProcessorInterface();
			if (!vp) return SetErrorAndReturn(ctx, "No VisionProcessor");

			if (!vp->IsValid(VMF::Measure))
			{
				if (IsDeadlineExpired())
				{
					if (m_retryCount < m_maxRetry)
					{
						m_retryCount++;
						EnterState(SendInspRequest);
						return VMF::TR_KEEP;
					}
					return SetErrorAndReturn(ctx, "1102 result timeout");
				}
				return VMF::TR_KEEP;
			}

			EnterState(ProcessResult);
			return VMF::TR_KEEP;
		}

		// ── 결과 처리 ────────────────────────────────────────────
		VMF::TaskResult C6SideInspectionTask::HandleProcessResult(
			VMF::Context& ctx, VMF::IActuator* actuator)
		{
			auto vp = ctx.GetVisionProcessorInterface();
			if (!vp) return SetErrorAndReturn(ctx, "No VisionProcessor");

			auto& data = vp->GetLatestData(VMF::Measure);

			// Grab Check
			auto itGrab = data.find("GrabCheck");
			if (itGrab != data.end() && itGrab->second == "2")
				return SetErrorAndReturn(ctx, "Grab failed on face " +
					std::to_string(m_facePosition));

			// Inspection Result (1:OK / 2:NG)
			auto itResult = data.find("InspectionResult");
			std::string result = "1";
			if (itResult != data.end()) result = itResult->second;

			// 면별 결과를 Context에 저장
			// ex) "Face_1_Result" = "1" (OK)
			std::string key = "Face_" + std::to_string(m_facePosition) + "_Result";
			ctx.SetSeqParam(key, result == "1" ? 1 : 0);

			if (result == "2")
				return SetErrorAndReturn(ctx, "Inspection NG on face " +
					std::to_string(m_facePosition));

			EnterState(TurnOffLight);
			return VMF::TR_KEEP;
		}

		VMF::TaskResult C6SideInspectionTask::HandleTurnOffLight(
			VMF::Context& ctx, VMF::IActuator* actuator)
		{
			if (actuator) actuator->SetLightState(m_cameraIndex, false);
			EnterState(Done);
			return VMF::TR_KEEP;
		}

	} // namespace Task
} // namespace VMF_6SIDE
