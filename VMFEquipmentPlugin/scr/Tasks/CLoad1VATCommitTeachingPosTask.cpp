#include "pch.h"
#include <algorithm>
#include "CLoad1VATCommitTeachingPosTask.h"
#include "DefineVAT.h"

using namespace VAT_LOAD1::Task;

CLoad1VATCommitTeachingPosTask::CLoad1VATCommitTeachingPosTask()
	: m_pickerCameraOffsetX_Narrow(0.0)
	, m_pickerCameraOffsetY_Narrow(0.0)
	, m_pickerCameraOffsetX_Wide(0.0)
	, m_pickerCameraOffsetY_Wide(0.0)
{
}

CLoad1VATCommitTeachingPosTask::~CLoad1VATCommitTeachingPosTask()
{
}

void CLoad1VATCommitTeachingPosTask::OnInitialize(VMF::Context& ctx)
{
	EnterState(LoadPickerCameraOffset);
}

VMF::TaskResult CLoad1VATCommitTeachingPosTask::OnPoll(
	VMF::Context& ctx,
	VMF::IActuator* actuator)
{
	switch (GetState())
	{
	case LoadPickerCameraOffset:    return HandleLoadPickerCameraOffset(ctx, actuator);
	case LoadVisionPositions:       return HandleLoadVisionPositions(ctx, actuator);
	case LoadHandPitchOffsets:      return HandleLoadHandPitchOffsets(ctx, actuator);
	case SaveTeachingPositions:     return HandleSaveTeachingPositions(ctx, actuator);
	default:                        return VMF::TR_ERROR;
	}
}

VMF::TaskResult CLoad1VATCommitTeachingPosTask::HandleLoadPickerCameraOffset(
	VMF::Context& ctx,
	VMF::IActuator* actuator)
{
	auto repo = ctx.GetRepository();

	if (!repo)
	{
		return SetErrorAndReturn(ctx, "CommitTeachingPos: Repository null");
	}

	const int cameraId = ctx.GetSeqParamAs<int>(VAT_SEQ_PARAM_CAMERA_INDEX, 0);
	const int packageId = ctx.GetSeqParamAs<int>(VAT_SEQ_PARAM_PACKAGE_ID, 0);

	if (repo->LoadPickerCamDistance(
		cameraId,
		packageId,
		m_pickerCameraOffsetX_Narrow,
		m_pickerCameraOffsetY_Narrow,
		m_pickerCameraOffsetX_Wide,
		m_pickerCameraOffsetY_Wide) != VMF::StorageSuccess)
	{
		return SetErrorAndReturn(ctx, "CommitTeachingPos: Can Not Load Cam Picker Distance");
	}

	EnterState(LoadVisionPositions);
	return VMF::TR_KEEP;
}

VMF::TaskResult CLoad1VATCommitTeachingPosTask::HandleLoadVisionPositions(
	VMF::Context& ctx,
	VMF::IActuator* actuator)
{
	m_teachingPositions = ctx.GetVisionPositions();

	EnterState(LoadHandPitchOffsets);
	return VMF::TR_KEEP;
}

VMF::TaskResult CLoad1VATCommitTeachingPosTask::HandleLoadHandPitchOffsets(
	VMF::Context& ctx,
	VMF::IActuator* actuator)
{
	const int handId = ctx.GetSeqParamAs<int>(VAT_SEQ_PARAM_HAND_ID, 0);
	const int packageId = ctx.GetSeqParamAs<int>(VAT_SEQ_PARAM_PACKAGE_ID, 0);

	const int pickerMaxRow = ctx.GetSeqParamAs<int>(VAT_SEQ_PARAM_PICKER_MAX_ROW, 0);
	const int pickerMaxCol = ctx.GetSeqParamAs<int>(VAT_SEQ_PARAM_PICKER_MAX_COL, 0);

	m_handPitchOffsets.clear();

	auto repo = ctx.GetRepository();
	if (!repo)
	{
		return SetErrorAndReturn(ctx, "CommitTeachingPos: Repository null");
	}

	for (int row = 0; row < pickerMaxRow; ++row)
	{
		const int cols[2] = { 0, pickerMaxCol - 1 };

		for (int i = 0; i < 2; ++i)
		{
			const int col = cols[i];

			double narrowOffsetX = 0.0;
			double narrowOffsetY = 0.0;
			double wideOffsetX = 0.0;
			double wideOffsetY = 0.0;

			if (repo->LoadHandPitch(
				handId,
				packageId,
				row,
				col,
				narrowOffsetX,
				narrowOffsetY,
				wideOffsetX,
				wideOffsetY) != VMF::StorageSuccess)
			{
				continue;
			}

			HandPitchOffset offset;
			offset.row = row;
			offset.col = col;
			offset.narrowOffsetX = narrowOffsetX;
			offset.narrowOffsetY = narrowOffsetY;
			offset.wideOffsetX = wideOffsetX;
			offset.wideOffsetY = wideOffsetY;

			m_handPitchOffsets.push_back(offset);
		}
	}

	if (m_handPitchOffsets.empty())
	{
		return SetErrorAndReturn(ctx, "CommitTeachingPos: HandPitch data empty");
	}

	EnterState(SaveTeachingPositions);
	return VMF::TR_KEEP;
}

VMF::TaskResult CLoad1VATCommitTeachingPosTask::HandleSaveTeachingPositions(
	VMF::Context& ctx,
	VMF::IActuator* actuator)
{
	const int handId = ctx.GetSeqParamAs<int>(VAT_SEQ_PARAM_HAND_ID, 0);
	const int packageId = ctx.GetSeqParamAs<int>(VAT_SEQ_PARAM_PACKAGE_ID, 0);

	std::vector<double> pitchDeltaX;
	std::vector<double> pitchDeltaY;

	std::vector<double> narrowPitchOffsetsX;
	std::vector<double> narrowPitchOffsetsY;
	std::vector<double> widePitchOffsetsX;
	std::vector<double> widePitchOffsetsY;

	for (size_t i = 0; i < m_handPitchOffsets.size(); ++i)
	{
		const HandPitchOffset& offset = m_handPitchOffsets[i];

		narrowPitchOffsetsX.push_back(offset.narrowOffsetX);
		narrowPitchOffsetsY.push_back(offset.narrowOffsetY);
		widePitchOffsetsX.push_back(offset.wideOffsetX);
		widePitchOffsetsY.push_back(offset.wideOffsetY);

		pitchDeltaX.push_back(offset.narrowOffsetX - offset.wideOffsetX);
		pitchDeltaY.push_back(offset.narrowOffsetY - offset.wideOffsetY);
	}

	auto minmaxPitchDeltaX = std::minmax_element(pitchDeltaX.begin(), pitchDeltaX.end());
	auto minmaxPitchDeltaY = std::minmax_element(pitchDeltaY.begin(), pitchDeltaY.end());

	const double pitchDeltaCenterX = (*minmaxPitchDeltaX.first + *minmaxPitchDeltaX.second) * 0.5;
	const double pitchDeltaCenterY = (*minmaxPitchDeltaY.first + *minmaxPitchDeltaY.second) * 0.5;

	auto narrowMinMaxX = std::minmax_element(narrowPitchOffsetsX.begin(), narrowPitchOffsetsX.end());
	auto narrowMinMaxY = std::minmax_element(narrowPitchOffsetsY.begin(), narrowPitchOffsetsY.end());

	auto wideMinMaxX = std::minmax_element(widePitchOffsetsX.begin(), widePitchOffsetsX.end());
	auto wideMinMaxY = std::minmax_element(widePitchOffsetsY.begin(), widePitchOffsetsY.end());

	const double narrowPitchCenterX = (*narrowMinMaxX.first + *narrowMinMaxX.second) * 0.5;
	const double narrowPitchCenterY = (*narrowMinMaxY.first + *narrowMinMaxY.second) * 0.5;

	const double widePitchCenterX = (*wideMinMaxX.first + *wideMinMaxX.second) * 0.5;
	const double widePitchCenterY = (*wideMinMaxY.first + *wideMinMaxY.second) * 0.5;

	const double narrowCalibrationX = narrowPitchCenterX + pitchDeltaCenterX;
	const double narrowCalibrationY = narrowPitchCenterY + pitchDeltaCenterY;

	const double wideCalibrationX = widePitchCenterX - pitchDeltaCenterX;
	const double wideCalibrationY = widePitchCenterY - pitchDeltaCenterY;

	const double pickerGapX = ctx.GetSeqParamAs<double>(VAT_SEQ_PARAM_PICKER_GAP_X_NARROW, 0.0);
	const double pickerGapY = ctx.GetSeqParamAs<double>(VAT_SEQ_PARAM_PICKER_GAP_Y_NARROW, 0.0);

	auto repo = ctx.GetRepository();
	if (!repo)
	{
		return SetErrorAndReturn(ctx, "CommitTeachingPos: Repository null");
	}

	for (size_t i = 0; i < m_teachingPositions.size(); ++i)
	{
		VMF::VisionPosition& position = m_teachingPositions[i];

		if (position.locateId == LoadTable1 || position.locateId == LoadTable2)
		{
			position.pos[0] = position.pos[0] + m_pickerCameraOffsetX_Narrow + narrowCalibrationX;
			position.pos[1] = position.pos[1] + m_pickerCameraOffsetY_Narrow + narrowCalibrationY;
		}
		else
		{
			position.pos[0] = position.pos[0] + m_pickerCameraOffsetX_Narrow + narrowPitchCenterX;
			position.pos[1] = position.pos[1] + m_pickerCameraOffsetY_Narrow + narrowPitchCenterY;

			position.pos[0] -= pickerGapX / 2;
			position.pos[1] -= (pickerGapY + (pickerGapY / 2));
		}

		if (repo->SaveTeachingResult(
			handId,
			position.locateId,
			packageId,
			position.pos[0],
			position.pos[1],
			position.pos[2]) != VMF::StorageSuccess)
		{
			return SetErrorAndReturn(ctx, "CommitTeachingPos: Teaching Pos Save Fail");
		}
	}

	return VMF::TR_NEXT;
}

