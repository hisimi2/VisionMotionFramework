#pragma once
#include "NonBlockingTaskBase.h"

namespace VMF_PLUGIN
{
	// ----------------------------------------------------------------
	// CSixSideInspectionTask
	//
	// 1102 H→V: 해당 면 Vision 검사 요청
	//   Data[0] = 면 번호 (1~6)
	//   BarcodeID, LotID 포함
	// 1102 V→H: 결과 수신
	//   GrabCheck, InspectionResult, BarcodeID, LotID
	//
	// Tag 파라미터:
	//   "FacePosition" ? 면 번호 (1~6)
	//   "BarcodeID"    ? 모듈 S/N
	//   "LotID"        ? Lot ID
	//   "SelectCount"  ? 선택된 면 총 개수
	// ----------------------------------------------------------------
	class CSixSideInspectionTask : public VMF::NonBlockingTaskBase
	{
		enum Substep
		{
			Idle = 0,
			TurnOnLight,        // 조명 ON
			SendInspRequest,    // 1102 H→V 요청
			WaitInspResult,     // 1102 V→H 수신 대기
			ProcessResult,      // 결과 처리 및 저장
			TurnOffLight,       // 조명 OFF
			Done,
		};

		VMF::TaskResult HandleTurnOnLight(VMF::Context& ctx, VMF::IActuator* actuator);
		VMF::TaskResult HandleSendInspRequest(VMF::Context& ctx, VMF::IActuator* actuator);
		VMF::TaskResult HandleWaitInspResult(VMF::Context& ctx, VMF::IActuator* actuator);
		VMF::TaskResult HandleProcessResult(VMF::Context& ctx, VMF::IActuator* actuator);
		VMF::TaskResult HandleTurnOffLight(VMF::Context& ctx, VMF::IActuator* actuator);

	public:
		CSixSideInspectionTask();
		~CSixSideInspectionTask() override;

		std::string GetName() const override
		{
			return "Task_6Side_Inspection";
		}

	protected:
		void            OnInitialize(VMF::Context& ctx) override;
		VMF::TaskResult OnPoll(VMF::Context& ctx, VMF::IActuator* actuator) override;

	private:
		int         m_facePosition;   // 1~6
		std::string m_barcodeId;
		std::string m_lotId;
		int         m_selectCount;
		int         m_cameraIndex;
		long        m_timeoutMs;
		int         m_retryCount;
		int         m_maxRetry;
	};

} // namespace VMF_PLUGIN_6SIDE
