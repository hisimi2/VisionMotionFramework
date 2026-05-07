#include "StdAfx.h"
#include "CLoad1PerformFocusScanningTask.h"
#include "DVH_VAT/DefineVAT.h"

using namespace VAT_LOAD1::Task;

CLoad1PerformFocusScanningTask::CLoad1PerformFocusScanningTask()
	: m_cameraId(0)
	, m_packageId(0)
	, m_moveTimeoutMs(7000)
	, m_visionTimeoutMs(60000)
{
}

CLoad1PerformFocusScanningTask::~CLoad1PerformFocusScanningTask()
{
}

void CLoad1PerformFocusScanningTask::OnInitialize(DVH_VAT::VAT_Context& ctx)
{
	m_cameraId = ctx.GetSeqParamAs<int>(VAT_SEQ_PARAM_CAMERA_INDEX, 0);
	m_packageId = ctx.GetSeqParamAs<int>(VAT_SEQ_PARAM_PACKAGE_ID, 0);

	m_locationIds.clear();

	EnterState(MoveDown);
}

DVH_VAT::TaskResult CLoad1PerformFocusScanningTask::OnPoll(
	DVH_VAT::VAT_Context& ctx,
	DVH_VAT::IVatActuator* actuator)
{
	switch (GetState())
	{
	case MoveDown:        return HandleMoveDown(ctx, actuator);
	case MoveWait:        return HandleMoveWait(ctx, actuator);
	case VisionRequest:   return HandleVisionRequest(ctx, actuator);
	case VisionWait:      return HandleVisionWait(ctx, actuator);
	case ReturnHome:      return HandleReturnHome(ctx, actuator);
	case SaveFocusResult: return HandleSaveFocusResult(ctx);
	default:              return DVH_VAT::TR_ERROR;
	}
}

DVH_VAT::TaskResult CLoad1PerformFocusScanningTask::HandleMoveDown(
	DVH_VAT::VAT_Context& ctx,
	DVH_VAT::IVatActuator* actuator)
{
	if (!actuator)
		return SetErrorAndReturn(ctx, "Z down failed");

	DVH_VAT::VisionPosition position;
	if (!ctx.PeekVisionPosition(position))
	{
		return SetErrorAndReturn(ctx, "FocusScanning: Get Position Failed");
	}

	if (actuator->MoveZ(position.pos[2]) != DVH_VAT::ActOk)
		return SetErrorAndReturn(ctx, "Z down failed");

	EnterStateWithTimeout(MoveWait, m_moveTimeoutMs);
	return DVH_VAT::TR_KEEP;
}

DVH_VAT::TaskResult CLoad1PerformFocusScanningTask::HandleMoveWait(
	DVH_VAT::VAT_Context& ctx,
	DVH_VAT::IVatActuator* actuator)
{
	if (!actuator)
		return SetErrorAndReturn(ctx, "Z Down Fail");

	DVH_VAT::VisionPosition position;
	if (!ctx.PeekVisionPosition(position))
	{
		return SetErrorAndReturn(ctx, "FocusScanning: Get Position Failed");
	}

	if (actuator->isMoveZ(position.pos[2]) != DVH_VAT::ActOk)
	{
		if (IsDeadlineExpired())
			return SetErrorAndReturn(ctx, "Z Down Fail");

		return DVH_VAT::TR_KEEP;
	}

	EnterStateWithTimeout(VisionRequest, m_moveTimeoutMs);
	return DVH_VAT::TR_KEEP;
}

DVH_VAT::TaskResult CLoad1PerformFocusScanningTask::HandleVisionRequest(
	DVH_VAT::VAT_Context& ctx,
	DVH_VAT::IVatActuator* actuator)
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

	if (!ctx.ExecuteVisionCommand(DVH_VAT::Measure))
	{
		return SetErrorAndReturn(ctx, "Vision Command Failed");
	}

	EnterStateWithTimeout(VisionWait, m_visionTimeoutMs);
	return DVH_VAT::TR_KEEP;
}

DVH_VAT::TaskResult CLoad1PerformFocusScanningTask::HandleVisionWait(
	DVH_VAT::VAT_Context& ctx,
	DVH_VAT::IVatActuator* actuator)
{
	auto visionProcessor = ctx.GetVisionProcessorInterface();
	if (!visionProcessor)
		return SetErrorAndReturn(ctx, "No Vision Processor");

	if (!visionProcessor->IsValid(DVH_VAT::Measure))
	{
		if (IsDeadlineExpired())
			return SetErrorAndReturn(ctx, "Vision Time Out");

		return DVH_VAT::TR_KEEP;
	}

	std::map<std::string, std::string>& data = visionProcessor->GetLatestData(DVH_VAT::Measure);
	(void)data;

	if (actuator)
	{
		actuator->SetLightState(m_cameraId, false);
	}

	EnterState(ReturnHome);
	return DVH_VAT::TR_KEEP;
}

DVH_VAT::TaskResult CLoad1PerformFocusScanningTask::HandleReturnHome(
	DVH_VAT::VAT_Context& ctx,
	DVH_VAT::IVatActuator* actuator)
{
	if (!actuator)
		return SetErrorAndReturn(ctx, "Z Home Return Fail");

	if (actuator->MoveZ(0.0) != DVH_VAT::ActOk)
		return SetErrorAndReturn(ctx, "Z Home Return Fail");

	EnterStateWithTimeout(SaveFocusResult, m_moveTimeoutMs);
	return DVH_VAT::TR_KEEP;
}

DVH_VAT::TaskResult CLoad1PerformFocusScanningTask::HandleSaveFocusResult(
	DVH_VAT::VAT_Context& ctx)
{
	auto repo = ctx.getRepository();
	if (!repo)
	{
		return SetErrorAndReturn(ctx, "DB Access Fail");
	}

	DVH_VAT::VisionPosition position;
	if (!ctx.PopVisionPosition(position))
	{
		return SetErrorAndReturn(ctx, "FocusScanning: Pop Position Failed");
	}

	const int locationId = position.locateId;
	const double bestZPlateJig = position.pos[2];

	if (repo->LoadCamLocationGroup(m_cameraId, m_locationIds) != DVH_VAT::StorageSuccess)
	{
		return SetErrorAndReturn(ctx, "DB Read Fail");
	}

	if (m_cameraId > 5)
	{
		if (repo->SaveZFocusResult(m_cameraId, UpperTarget, m_packageId, bestZPlateJig) != DVH_VAT::StorageSuccess)
		{
			return SetErrorAndReturn(ctx, "DB Write Fail");
		}

		if (repo->SaveZFocusResult(m_cameraId, TargetA, m_packageId, bestZPlateJig) != DVH_VAT::StorageSuccess)
		{
			return SetErrorAndReturn(ctx, "DB Write Fail");
		}

		if (repo->SaveZFocusResult(m_cameraId, TargetB, m_packageId, bestZPlateJig) != DVH_VAT::StorageSuccess)
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

			if (repo->LoadInspInitPos(6, UpperTarget, m_packageId, dummy, dummy, upperTargetBestZ) != DVH_VAT::StorageSuccess)
			{
				return SetErrorAndReturn(ctx, "DB Read Fail");
			}

			bestZCok = upperTargetBestZ - 9.0;
		}

		if (repo->SaveZFocusResult(m_cameraId, *it, m_packageId, bestZCok) != DVH_VAT::StorageSuccess)
		{
			return SetErrorAndReturn(ctx, "DB Write Fail");
		}
	}

	if (ctx.IsVisionPositionEmpty())
	{
		return DVH_VAT::TR_NEXT;
	}

	(void)locationId;
	EnterState(MoveDown);
	return DVH_VAT::TR_PREV;
}

