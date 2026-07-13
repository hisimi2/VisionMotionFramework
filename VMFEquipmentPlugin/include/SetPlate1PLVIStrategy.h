#pragma once
#include "ComponentSetupBase.h"
#include "SqliteDataRepository.h"
#include "..\VMFEquipmentPlugin\Protocol\VisionMemoryProcessor.h"
#include "..\VMFEquipmentPlugin\Protocol\VisionPLVIProcessor.h"
#include "Sequences\CSetPlate1PLVISequenceBuilder.h"
#include <memory>

namespace VMF_PLVI_SETPLATE1
{
	using namespace VMF;

	class SetPlate1PLVIStrategy : public VMF::ComponentSetupBase
	{
	public:
		std::string GetSequenceName() const { return "SetPlate1PLVI"; }

		VMF::DataRepositoryPtr CreateRepository() override
		{
			auto repo = std::make_shared<VMF::SqliteDataRepository>("Data\\PLVI_DATABASE.db", "Data\\Images");
			repo->Initialize();
			return repo;
		}

		VMF::VisionProcessorPtr CreateVisionProcessor() override
		{
			VisionConnectionConfig config("127.0.0.1", 8000, 3000);
			auto vm = std::make_shared<VMF::VisionPLVIProcessor>();
			// auto vm = std::make_shared<VMF::VisionPLVIProcessor>();
			vm->Initialize(config);
			return vm;
		}

		VMF::SequenceBuilderPtr CreateBuilder() override
		{
			return std::make_shared<VMF_PLVI::Sequence::CSetPlate1PLVISequenceBuilder>();
		}

		void ConfigureParams(VMF::VisionContextPtr ctx) override
		{
			auto repo = ctx->GetRepository();
			VMF::VisionParams params;

			// 시퀀스 관련 파라미터
			SetParam(params, "HandID", 1);
			SetParam(params, "PkgID", 1);
			SetParam(params, "PLVI_POSITION", 0);
			SetParam(params, "TIMEOUT_MEASURE_MS", 5000);
			SetParam(params, "TIMEOUT_SCAN_MS", 15000);
			SetParam(params, "TIMEOUT_RESULT_MS", 10000);
			SetParam(params, "TIMEOUT_MOVE_MS", 7000);
			SetParam(params, "SCAN_SPEED_MM_S", 100.0);
			SetParam(params, "TRIGGER_INTERVAL_MM", 2.0);

			// Vision 관련 
			SetParam(params, "DATA_ID", 1);
			SetParam(params, "PKG_NAME", "TEST_PKG");

			int ctrayX = 8, ctrayY = 4;
			SetParam(params, "CTRAY_X", ctrayX);
			SetParam(params, "CTRAY_Y", ctrayY);

			// Handler 물류 정보 초기화 (전체 Device 존재 = 99)
			const int totalPockets = ctrayX * ctrayY;
			for (int i = 0; i < totalPockets; ++i)
			{
				std::string key = "DEVICE_INFO_" + std::to_string(i);
				SetParam(params, key.c_str(), 99);
			}

			// visionPositions - LoadResult(InspInitPos) 사용
			const int nLocateId = 0;
			const int nVisionRequestId = 1;
			const int pkgId = 1;

			double scanStartX = 0.0, scanStartY = 0.0, scanStartZ = 0.0;

			/*if (repo)
			{
				StringMap q;
				q[CAM_INDEX] = "0";    // PLVI 전용 카메라
				q[LOCATION_ID] = std::to_string(nLocateId);
				q[PKG_ID] = std::to_string(pkgId);

				StringMap out;
				if (repo->LoadResult(InspInitPos, q, out) == StorageSuccess)
				{
					scanStartX = std::atof(out[POS_X].c_str());
					scanStartY = std::atof(out[POS_Y].c_str());
					scanStartZ = std::atof(out[FOCUS].c_str());
				}
			}*/

			AddVisionPoint(params, nLocateId, nVisionRequestId,
				scanStartX, scanStartY, scanStartZ);

			ctx->SetVisionParams(params);
		}

		// ----------------------------------------------------------------
		// GetVisionParams - Preset별 카메라 파라미터
		// ----------------------------------------------------------------
		VMF::StringMap GetVisionParams(const std::string& presetName) const override
		{
			StringMap p;

			return StringMap();
		}
	};

} // namespace PLVI_SETPLATE1