#pragma once
#include "IVisionProcessor.h"
#include "Types.h"

#include <map>
#include <mutex>
#include <string>
#include <vector>

namespace VMF
{
	class CMockVisionEventHandler : public IVisionProcessor
	{
	public:
		CMockVisionEventHandler();
		virtual ~CMockVisionEventHandler();

		// IAsyncVisionProcessor
		virtual VC::Status Initialize(const VisionConnectionConfig& config);
		virtual void Disconnect();
		virtual bool IsConnected() const;

		virtual bool RequestSetCokAsync(const StringMap& params);
		virtual bool RequestInspReadyAsync(const StringMap& params);
		virtual bool RequestMeasureAsync(const StringMap& params);
		virtual bool RequestDeviceCheckAsync(const StringMap& params);
		virtual bool RequestLightAsync(const StringMap& params);

		virtual DataMap GetLatestData(VisionCommand type) const;
		virtual void ClearLatestData(VisionCommand type);
		virtual bool IsValid(VisionCommand type) const;
		virtual bool HasReceived(VisionCommand type) const;

		// IVisionEventHandler (?섏떊 肄쒕갚)
		virtual void InitializeRecvThread();

		virtual void OnSetCok(ByteArray body);
		virtual void OnInspReady(ByteArray body);
		virtual void OnMeasure(ByteArray body);
		virtual void OnDeviceCheck(ByteArray body);
		virtual void OnLight(ByteArray body);

		// ?뚯뒪???ы띁
		void SetRequestResult(bool ok);
		StringMap GetLastRequestParams() const;

	private:
		mutable std::mutex m_mutex;
		bool m_connected;
		bool m_requestResult; 
						
		std::map<int, StringMap> m_latestData;

		std::map<int, bool> m_receivedFlags;

		StringMap m_lastRequestParams;

		static std::string BodyToString(const ByteArray& b);
	};
} // namespace VMF

