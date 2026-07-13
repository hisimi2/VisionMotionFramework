#pragma once
#include "NonBlockingTaskBase.h"

namespace VMF_PLVI
{
	namespace Task
	{
		class CSetPlate1PLVIPerformScanTask : public VMF::NonBlockingTaskBase
		{

			// PLVI 파싱 결과 구조체
			// pocketStatus[row][col] = PLVIPocketCode 값 (0/1/2/11/12/99)
			struct PLVIStatus
			{
				bool resultOK = false;
				int  overallResult = 1;    // cData[1]: 0=OK, 1=NG
				int  plviPosition = 0;    // cData[2]: 검사 위치 코드
				int  rows = 0;
				int  cols = 0;

				int  normalCount = 0;   // 99
				int  emptyCount = 0;   // 0
				int  leaveCount = 0;   // 1
				int  doubleCount = 0;   // 2
				int  missingCount = 0;   // 11
				int  mismatchCount = 0;   // 12

										  // [row][col] = PLVIPocketCode
				std::vector<std::vector<int>> pocketStatus;
			};

			enum Substep
			{
				RequestMeasure = 0,
				WaitMeasureAck,
				PerformScan,
				WaitScanComplete,
				RequestResult,
				WaitResult,
				SaveResult,
			};

			VMF::TaskResult HandleRequestMeasure(VMF::Context& ctx, VMF::IActuator* actuator);
			VMF::TaskResult HandleWaitMeasureAck(VMF::Context& ctx, VMF::IActuator* actuator);
			VMF::TaskResult HandlePerformScan(VMF::Context& ctx, VMF::IActuator* actuator);
			VMF::TaskResult HandleWaitScanComplete(VMF::Context& ctx, VMF::IActuator* actuator);
			VMF::TaskResult HandleRequestResult(VMF::Context& ctx, VMF::IActuator* actuator);
			VMF::TaskResult HandleWaitResult(VMF::Context& ctx, VMF::IActuator* actuator);
			VMF::TaskResult HandleSaveResult(VMF::Context& ctx, VMF::IActuator* actuator);

		public:
			CSetPlate1PLVIPerformScanTask();
			virtual ~CSetPlate1PLVIPerformScanTask();

			std::string GetName() const override
			{
				return "Task_PLVI_PerformScan";
			}

		protected:
			void            OnInitialize(VMF::Context& ctx) override;
			VMF::TaskResult OnPoll(VMF::Context& ctx,
				VMF::IActuator* actuator) override;

			// StringMap → PLVIResult 2D 배열 원복
			PLVIStatus ParsePLVIResult(const VMF::StringMap& data);

			int    m_plviPosition;
			int    m_ctrayX;
			int    m_ctrayY;
			int    m_dataId;
			double m_scanEndY;
			double m_scanSpeedMmS;

			// Retry
			int    m_retryCount;
			int    m_maxRetryCount;

			// 타임아웃
			long   m_timeoutMeasureMs;
			long   m_timeoutScanMs;
			long   m_timeoutResultMs;
			long   m_timeoutMoveMs;

			std::vector<double> m_scanStartPos;


            VMF::VisionPosition m_measurepos;
		};

	} // namespace Task
} // namespace VMF_PLVI
