#pragma once
#include "ComponentSetupBase.h"
#include "DefaultSetupStrategy.h"
#include "SqliteDataRepository.h"
// VisionMemoryProcessor는 여기서 사용하지 않음 (제거)
#include "..\scr\Protocol\VisionPLVIProcessor.h"
#include "..\scr\Strategies\CSetPlate1PLVISequenceBuilder.h"
#include <memory>
#include <sstream>

namespace VMF_PLUGIN
{
	class SetPlate1PLVIStrategy : public VMF::DefaultSetupStrategy
	{
	protected:
		// -----------------------------------------------------------------
		// Helper: VisionParams??臾몄옄???뚮씪誘명꽣 ?ㅼ젙
		// -----------------------------------------------------------------
		void SetParam(VMF::VisionParams& params, const std::string& key, const std::string& value)
		{
			params.visionParams[key] = value;
		}

		// Helper: VisionParams???뺤닔 ?뚮씪誘명꽣 ?ㅼ젙 (?먮룞 臾몄옄??蹂??
		void SetParam(VMF::VisionParams& params, const std::string& key, int value)
		{
			params.visionParams[key] = std::to_string(value);
		}

		// Helper: VisionParams???ㅼ닔 ?뚮씪誘명꽣 ?ㅼ젙 (?먮룞 臾몄옄??蹂??
		void SetParam(VMF::VisionParams& params, const std::string& key, double value)
		{
			std::ostringstream oss;
			oss << value;
			params.visionParams[key] = oss.str();
		}

		// -----------------------------------------------------------------
		// Helper: VisionPosition 異붽?
		// -----------------------------------------------------------------
		void AddVisionPoint(VMF::VisionParams& params, int locateId, int requestId,
			double x, double y, double z)
		{
			std::vector<double> pos;
			pos.push_back(x);
			pos.push_back(y);
			pos.push_back(z);
			params.visionPositions.push_back(
				VMF::VisionPosition(pos, locateId, requestId));
		}

		// -----------------------------------------------------------------
		// Helper: VisionParams瑜?Repository?????
		// -----------------------------------------------------------------
		void SaveVisionParamsToRepo(VMF::VisionContextPtr ctx,
			const VMF::VisionParams& params)
		{
			auto repo = ctx->GetRepository();
			if (!repo)
				return;

			// visionParams??紐⑤뱺 ??媛??띿쓣 Repository?????
			for (const auto& kv : params.visionParams)
			{
				repo->SaveParam("PLVI", kv.first, kv.second);
			}
		}

	public:
		std::string GetSequenceName() const override { return "SetPlate1PLVI"; }

		VMF::DataRepositoryPtr CreateRepository() override
		{
			auto repo = std::make_shared<VMF::SqliteDataRepository>(
				"Data\\PLVI_DATABASE.db", "Data\\Images");
			repo->Initialize();
			return repo;
		}

		VMF::VisionProcessorPtr CreateVisionProcessor() override
		{
			VMF::VisionConnectionConfig config("127.0.0.1", 8000, 3000);
			auto vm = std::make_shared<VMF::VisionPLVIProcessor>();
			vm->Initialize(config);
			return vm;
		}

		VMF::SequenceBuilderPtr CreateBuilder() override
		{
			return std::make_shared<VMF_PLUGIN::CSetPlate1PLVISequenceBuilder>();
		}

		void ConfigureParams(VMF::VisionContextPtr ctx) override
		{
			auto repo = ctx->GetRepository();
			VMF::VisionParams params;

			// ?쒗??愿???뚮씪誘명꽣
			SetParam(params, "HandID", 1);
			SetParam(params, "PkgID", 1);
			SetParam(params, "PLVI_POSITION", 0);
			SetParam(params, "TIMEOUT_MEASURE_MS", 5000);
			SetParam(params, "TIMEOUT_SCAN_MS", 15000);
			SetParam(params, "TIMEOUT_RESULT_MS", 10000);
			SetParam(params, "TIMEOUT_MOVE_MS", 7000);
			SetParam(params, "SCAN_SPEED_MM_S", 100.0);
			SetParam(params, "TRIGGER_INTERVAL_MM", 2.0);

			// Vision 愿??
			SetParam(params, "DATA_ID", 1);
			SetParam(params, "PKG_NAME", "TEST_PKG");

			int ctrayX = 8, ctrayY = 4;
			SetParam(params, "CTRAY_X", ctrayX);
			SetParam(params, "CTRAY_Y", ctrayY);

			// Handler 臾쇰쪟 ?뺣낫 珥덇린??(?꾩껜 Device 議댁옱 = 99)
			const int totalPockets = ctrayX * ctrayY;
			for (int i = 0; i < totalPockets; ++i)
			{
				std::string key = "DEVICE_INFO_" + std::to_string(i);
				SetParam(params, key, 99);
			}

			// visionPositions - LoadResult(InspInitPos) ?ъ슜
			const int nLocateId = 0;
			const int nVisionRequestId = 1;
			const int pkgId = 1;

			double scanStartX = 0.0, scanStartY = 0.0, scanStartZ = 0.0;

			/*if (repo)
			{
				VMF::StringMap q;
				q[CAM_INDEX] = "0";    // PLVI ?꾩슜 移대찓??
				q[LOCATION_ID] = std::to_string(nLocateId);
				q[PKG_ID] = std::to_string(pkgId);

				VMF::StringMap out;
				if (repo->LoadResult(InspInitPos, q, out) == StorageSuccess)
				{
					scanStartX = std::atof(out[POS_X].c_str());
					scanStartY = std::atof(out[POS_Y].c_str());
					scanStartZ = std::atof(out[FOCUS].c_str());
				}
			}*/

			AddVisionPoint(params, nLocateId, nVisionRequestId,
				scanStartX, scanStartY, scanStartZ);

			// Repository??params ???
			SaveVisionParamsToRepo(ctx, params);
		}

		// ----------------------------------------------------------------
		// GetVisionParams - Preset蹂?移대찓???뚮씪誘명꽣
		// ----------------------------------------------------------------
		VMF::StringMap GetVisionParams(const std::string& presetName) const override
		{
			VMF::StringMap p;

			return VMF::StringMap();
		}
	};
} 
