#include "pch.h"
#include "CLoad1VATPerformFocusScanningTask.h"
#include "DefineVAT.h"

using namespace VAT_LOAD1::Task;

CLoad1VATPerformFocusScanningTask::CLoad1VATPerformFocusScanningTask()
	: m_cameraId(0)
	, m_packageId(0)
	, m_moveTimeoutMs(7000)
	, m_visionTimeoutMs(60000)
{
}

CLoad1VATPerformFocusScanningTask::~CLoad1VATPerformFocusScanningTask()
{
}

void CLoad1VATPerformFocusScanningTask::OnInitialize(VMF::Context& ctx)
{
	m_cameraId = GetTaskSeqParamAs<int>(ctx, VAT_SEQ_PARAM_CAMERA_INDEX, 0);
	m_packageId = GetTaskSeqParamAs<int>(ctx, VAT_SEQ_PARAM_PACKAGE_ID, 0);

	m_locationIds.clear();

	EnterState(MoveDown);
}

VMF::TaskResult CLoad1VATPerformFocusScanningTask::OnPoll(
	VMF::Context& ctx,
	VMF::IActuator* actuator)
{
	switch (GetState())
	{
	case MoveDown:        return HandleMoveDown(ctx, actuator);
	case MoveWait:        return HandleMoveWait(ctx, actuator);
	case VisionRequest:   return HandleVisionRequest(ctx, actuator);
	case VisionWait:      return HandleVisionWait(ctx, actuator);
	case ReturnHome:      return HandleReturnHome(ctx, actuator);
	case SaveFocusResult: return HandleSaveFocusResult(ctx);
	default:              return VMF::TR_ERROR;
	}
}

VMF::TaskResult CLoad1VATPerformFocusScanningTask::HandleMoveDown(
	VMF::Context& ctx,
	VMF::IActuator* actuator)
{
	if (!actuator)
		return SetErrorAndReturn(ctx, "Z down failed");

    /*
	VMF::VisionPosition position;
	if (!ctx.PeekVisionPosition(position))
	{
		return SetErrorAndReturn(ctx, "FocusScanning: Get Position Failed");
	}
    

	if (actuator->MoveZ(position.pos[2]) != VMF::ActOk)
		return SetErrorAndReturn(ctx, "Z down failed");

    */


	EnterStateWithTimeout(MoveWait, m_moveTimeoutMs);
	return VMF::TR_KEEP;
}

VMF::TaskResult CLoad1VATPerformFocusScanningTask::HandleMoveWait(
	VMF::Context& ctx,
	VMF::IActuator* actuator)
{
	if (!actuator)
		return SetErrorAndReturn(ctx, "Z Down Fail");


    /*
	VMF::VisionPosition position;
	if (!ctx.PeekVisionPosition(position))
	{
		return SetErrorAndReturn(ctx, "FocusScanning: Get Position Failed");
	}
    

	if (actuator->isMoveZ(position.pos[2]) != VMF::ActOk)
	{
		if (IsDeadlineExpired())
			return SetErrorAndReturn(ctx, "Z Down Fail");

		return VMF::TR_KEEP;
	}
    */

	EnterStateWithTimeout(VisionRequest, m_moveTimeoutMs);
	return VMF::TR_KEEP;
}

VMF::TaskResult CLoad1VATPerformFocusScanningTask::HandleVisionRequest(
	VMF::Context& ctx,
	VMF::IActuator* actuator)
{
	if (!actuator)
		return SetErrorAndReturn(ctx, "No Actuator");

	actuator->SetLightState(m_cameraId, true);

	auto visionProcessor = ctx.GetVisionProcessorInterface();
	if (!visionProcessor)
	{
		return SetErrorAndReturn(ctx, "No Vision Processor");
	}

	visionProcessor->InitializeRecvThread();

	if (!ctx.ExecuteVisionCommand(VMF::Measure))
	{
		return SetErrorAndReturn(ctx, "Vision Command Failed");
	}

	EnterStateWithTimeout(VisionWait, m_visionTimeoutMs);
	return VMF::TR_KEEP;
}

VMF::TaskResult CLoad1VATPerformFocusScanningTask::HandleVisionWait(
	VMF::Context& ctx,
	VMF::IActuator* actuator)
{
	auto visionProcessor = ctx.GetVisionProcessorInterface();
	if (!visionProcessor)
		return SetErrorAndReturn(ctx, "No Vision Processor");

	if (!visionProcessor->IsValid(VMF::Measure))
	{
		if (IsDeadlineExpired())
			return SetErrorAndReturn(ctx, "Vision Time Out");

		return VMF::TR_KEEP;
	}

    /*
	std::map<std::string, std::string>& data = visionProcessor->GetLatestData(VMF::Measure);
	(void)data;

	if (actuator)
	{
		actuator->SetLightState(m_cameraId, false);
	}
    */

	EnterState(ReturnHome);
	return VMF::TR_KEEP;
}

VMF::TaskResult CLoad1VATPerformFocusScanningTask::HandleReturnHome(
	VMF::Context& ctx,
	VMF::IActuator* actuator)
{
	if (!actuator)
		return SetErrorAndReturn(ctx, "Z Home Return Fail");

	if (actuator->MoveZ(0.0) != VMF::ActOk)
		return SetErrorAndReturn(ctx, "Z Home Return Fail");

	EnterStateWithTimeout(SaveFocusResult, m_moveTimeoutMs);
	return VMF::TR_KEEP;
}

VMF::TaskResult CLoad1VATPerformFocusScanningTask::HandleSaveFocusResult(
	VMF::Context& ctx)
{
	auto repo = ctx.GetRepository();
	if (!repo)
	{
		return SetErrorAndReturn(ctx, "DB Access Fail");
	}

    /*
	VMF::VisionPosition position;
	if (!ctx.PopVisionPosition(position))
	{
		return SetErrorAndReturn(ctx, "FocusScanning: Pop Position Failed");
	}
    

	const int locationId = position.locateId;
	const double bestZPlateJig = position.pos[2];

	if (repo->LoadCamLocationGroup(m_cameraId, m_locationIds) != VMF::StorageSuccess)
	{
		return SetErrorAndReturn(ctx, "DB Read Fail");
	}

    

	if (m_cameraId > 5)
	{
		if (repo->SaveZFocusResult(m_cameraId, UpperTarget, m_packageId, bestZPlateJig) != VMF::StorageSuccess)
		{
			return SetErrorAndReturn(ctx, "DB Write Fail");
		}

		if (repo->SaveZFocusResult(m_cameraId, TargetA, m_packageId, bestZPlateJig) != VMF::StorageSuccess)
		{
			return SetErrorAndReturn(ctx, "DB Write Fail");
		}

		if (repo->SaveZFocusResult(m_cameraId, TargetB, m_packageId, bestZPlateJig) != VMF::StorageSuccess)
		{
			return SetErrorAndReturn(ctx, "DB Write Fail");
		}
	}

    

	for (std::vector<int>::iterator it = m_locationIds.begin(); it != m_locationIds.end(); ++it)
	{
		double bestZCok = 0.0;

		if (*it < LoadTable1)
		{
			bestZCok = bestZPlateJig + 15.63;
		}
		else if (*it < Picker)
		{
			bestZCok = bestZPlateJig - 7.8;
		}
		else
		{
			double dummy = 0.0;
			double upperTargetBestZ = 0.0;

			if (repo->LoadInspInitPos(6, UpperTarget, m_packageId, dummy, dummy, upperTargetBestZ) != VMF::StorageSuccess)
			{
				return SetErrorAndReturn(ctx, "DB Read Fail");
			}

			bestZCok = upperTargetBestZ - 9.0;
		}

		if (repo->SaveZFocusResult(m_cameraId, *it, m_packageId, bestZCok) != VMF::StorageSuccess)
		{
			return SetErrorAndReturn(ctx, "DB Write Fail");
		}
	}

	if (ctx.IsVisionPositionEmpty())
	{
		return VMF::TR_NEXT;
	}

    

	(void)locationId;

    */
	EnterState(MoveDown);
	return VMF::TR_PREV;
}

