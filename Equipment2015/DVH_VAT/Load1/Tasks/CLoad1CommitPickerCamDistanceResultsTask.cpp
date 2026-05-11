#include "StdAfx.h"
#include "CLoad1CommitPickerCamDistanceResultsTask.h"
#include "DVH_VAT/DefineVAT.h"

using namespace VAT_LOAD1::Task;

CLoad1CommitPickerCamDistanceResultsTask::CLoad1CommitPickerCamDistanceResultsTask()
	: m_lowerCameraId(0)
	, m_packageId(0)
	, m_pickerHandId(0)
	, m_currentUpperCameraIndex(0)
	, m_pickerReferencePosX_Narrow(0.0)
	, m_pickerReferencePosY_Narrow(0.0)
	, m_pickerReferencePosX_Wide(0.0)
	, m_pickerReferencePosY_Wide(0.0)
	, m_currentPitchMode(VMF::Narrow)
{
}

CLoad1CommitPickerCamDistanceResultsTask::~CLoad1CommitPickerCamDistanceResultsTask()
{
}

void CLoad1CommitPickerCamDistanceResultsTask::OnInitialize(VMF::VAT_Context& ctx)
{
	m_currentUpperCameraIndex = 0;
	m_currentPitchMode = VMF::Narrow;

	m_lowerCameraId = ctx.GetSeqParamAs<int>(VAT_SEQ_PARAM_CAMERA_INDEX, 0);
	m_packageId = ctx.GetSeqParamAs<int>(VAT_SEQ_PARAM_PACKAGE_ID, 0);
	m_pickerHandId = ctx.GetSeqParamAs<int>(VAT_SEQ_PARAM_HAND_ID, 0);

	m_upperCameraIds.clear();

	auto repo = ctx.getRepository();

	if (repo && m_pickerHandId > 0)
	{
		std::vector<int> camList;
		if (repo->LoadHandCamGroup(m_pickerHandId, camList) == VMF::StorageSuccess)
		{
			for (std::vector<int>::iterator it = camList.begin(); it != camList.end(); ++it)
			{
				const int camId = *it;
				if (camId != m_lowerCameraId)
				{
					m_upperCameraIds.push_back(camId);
				}
			}
		}
	}

	EnterState(GetStdPickerPos);
}

VMF::TaskResult CLoad1CommitPickerCamDistanceResultsTask::OnPoll(
	VMF::VAT_Context& ctx,
	VMF::IVatActuator* actuator)
{
	switch (GetState())
	{
	case GetStdPickerPos: return HandleGetStdPickerPos(ctx, actuator);
	case CalcCamAlignPos: return HandleCalcCamAlignPos(ctx, actuator);
	default:              return VMF::TR_ERROR;
	}
}

VMF::TaskResult CLoad1CommitPickerCamDistanceResultsTask::HandleGetStdPickerPos(
	VMF::VAT_Context& ctx,
	VMF::IVatActuator* actuator)
{
	auto repo = ctx.getRepository();

	if (!repo)
		return SetErrorAndReturn(ctx, "Repository is null");

	int locateId = 0;
	double outFocusZ = 0.0;

	if (repo->LoadLocationIdByName("Picker", locateId) != VMF::StorageSuccess)
	{
		return SetErrorAndReturn(ctx, "Failed to load Picker location id");
	}

	if (m_currentPitchMode == VMF::Narrow)
	{
		if (repo->LoadInspInitPos(
			m_lowerCameraId,
			locateId,
			m_packageId,
			m_pickerReferencePosX_Narrow,
			m_pickerReferencePosY_Narrow,
			outFocusZ) != VMF::StorageSuccess)
		{
			return SetErrorAndReturn(ctx, "Failed to load Picker align position");
		}

		m_currentPitchMode = VMF::Wide;
		return VMF::TR_PREV;
	}

	if (repo->LoadInspInitPos(
		m_lowerCameraId,
		locateId,
		m_packageId,
		m_pickerReferencePosX_Wide,
		m_pickerReferencePosY_Wide,
		outFocusZ) != VMF::StorageSuccess)
	{
		return SetErrorAndReturn(ctx, "Failed to load Picker align position");
	}

	EnterState(CalcCamAlignPos);
	return VMF::TR_KEEP;
}

VMF::TaskResult CLoad1CommitPickerCamDistanceResultsTask::HandleCalcCamAlignPos(
	VMF::VAT_Context& ctx,
	VMF::IVatActuator* actuator)
{
	if (m_currentUpperCameraIndex >= m_upperCameraIds.size())
	{
		auto repo = ctx.getRepository();

		if (!repo)
			return SetErrorAndReturn(ctx, "Repository is null");

		int locateId = 0;

		repo->LoadLocationIdByName("Picker", locateId);
		repo->SaveCalibrationPosResult(
			m_lowerCameraId,
			locateId,
			m_packageId,
			m_pickerReferencePosX_Narrow,
			m_pickerReferencePosY_Narrow);

		repo->LoadLocationIdByName("Picker Wide", locateId);
		repo->SaveCalibrationPosResult(
			m_lowerCameraId,
			locateId,
			m_packageId,
			m_pickerReferencePosX_Wide,
			m_pickerReferencePosY_Wide);

		return VMF::TR_NEXT;
	}

	auto repo = ctx.getRepository();

	if (!repo)
		return SetErrorAndReturn(ctx, "Repository is null");

	const int currentUpperCameraId = m_upperCameraIds[m_currentUpperCameraIndex];

	int locateIdLowerTarget = 0;
	int locateIdUpperTarget = 0;
	int locateIdTargetA = 0;
	int locateIdTargetB = 0;

	repo->LoadLocationIdByName("Lower Target", locateIdLowerTarget);
	repo->LoadLocationIdByName("Upper Target", locateIdUpperTarget);
	repo->LoadLocationIdByName("Target A", locateIdTargetA);
	repo->LoadLocationIdByName("Target B", locateIdTargetB);

	double outLowTargetOffsetX = 0.0;
	double outLowTargetOffsetY = 0.0;
	double outFocusZ = 0.0;
	double outUpperTargetPosX = 0.0;
	double outUpperTargetPosY = 0.0;
	double outTargetA_X = 0.0;
	double outTargetA_Y = 0.0;
	double outTargetB_X = 0.0;
	double outTargetB_Y = 0.0;

	const double targetDiffX = ctx.GetSeqParamAs<double>(VAT_SEQ_PARAM_TARGET_DIFF_X, 0.0);
	const double targetDiffY = ctx.GetSeqParamAs<double>(VAT_SEQ_PARAM_TARGET_DIFF_Y, 0.0);

	repo->LoadInspInitPos(
		m_lowerCameraId,
		locateIdLowerTarget,
		m_packageId,
		outLowTargetOffsetX,
		outLowTargetOffsetY,
		outFocusZ);

	repo->LoadInspInitPos(
		currentUpperCameraId,
		locateIdUpperTarget,
		m_packageId,
		outUpperTargetPosX,
		outUpperTargetPosY,
		outFocusZ);

	repo->LoadInspInitPos(
		currentUpperCameraId,
		locateIdTargetA,
		m_packageId,
		outTargetA_X,
		outTargetA_Y,
		outFocusZ);

	repo->LoadInspInitPos(
		currentUpperCameraId,
		locateIdTargetB,
		m_packageId,
		outTargetB_X,
		outTargetB_Y,
		outFocusZ);

	const double jigTheta = atan2(
		(outTargetB_Y - outTargetA_Y),
		(outTargetB_X - outTargetA_X));

	const double realDiffX = targetDiffX * cos(jigTheta) - targetDiffY * sin(jigTheta);
	const double realDiffY = targetDiffX * sin(jigTheta) + targetDiffY * cos(jigTheta);

	const double camAlignPosX = outUpperTargetPosX + realDiffX + outLowTargetOffsetX;
	const double camAlignPosY = outUpperTargetPosY + realDiffY - outLowTargetOffsetY;

	const double pickerCamDistX_Narrow = m_pickerReferencePosX_Narrow - camAlignPosX;
	const double pickerCamDistY_Narrow = m_pickerReferencePosY_Narrow - camAlignPosY;
	const double pickerCamDistX_Wide = m_pickerReferencePosX_Wide - camAlignPosX;
	const double pickerCamDistY_Wide = m_pickerReferencePosY_Wide - camAlignPosY;

	repo->SavePickerCamDistanceResult(
		currentUpperCameraId,
		m_packageId,
		pickerCamDistX_Narrow,
		pickerCamDistY_Narrow,
		pickerCamDistX_Wide,
		pickerCamDistY_Wide);

	++m_currentUpperCameraIndex;
	return VMF::TR_KEEP;
}

