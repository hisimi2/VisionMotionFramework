#include "stdafx.h"
#include "Mock/CMockVisionClient.h"

namespace VMF
{
	CMockVisionClient::CMockVisionClient()
		: m_connected(false)
		, m_requestResult(true)
	{
	}

	CMockVisionClient::~CMockVisionClient()
	{
	}

	VC::Status CMockVisionClient::Initialize(const VisionConnectionConfig& /*config*/)
	{
		std::lock_guard<std::mutex> lg(m_mutex);
		m_connected = true;
		return VC::VisionOK;
	}

	VC::Status CMockVisionClient::InitializeWithSharedController(
		std::shared_ptr<VC::Controller> sharedCtrl,
		const VisionConnectionConfig& config)
	{
		std::lock_guard<std::mutex> lg(m_mutex);
		m_sharedCtrl = sharedCtrl;
		m_connected = true;
		return VC::VisionOK;
	}

	void CMockVisionClient::Disconnect()
	{
		std::lock_guard<std::mutex> lg(m_mutex);
		m_connected = false;
	}

	bool CMockVisionClient::IsConnected() const
	{
		std::lock_guard<std::mutex> lg(m_mutex);
		return m_connected;
	}

	bool CMockVisionClient::RequestSetCokAsync(const StringMap& params)
	{
		std::lock_guard<std::mutex> lg(m_mutex);
		m_lastRequestParams = params;
		return m_requestResult;
	}

	bool CMockVisionClient::RequestInspReadyAsync(const StringMap& params)
	{
		std::lock_guard<std::mutex> lg(m_mutex);
		m_lastRequestParams = params;
		return m_requestResult;
	}

	bool CMockVisionClient::RequestMeasureAsync(const StringMap& params)
	{
		std::lock_guard<std::mutex> lg(m_mutex);
		m_lastRequestParams = params;
		return m_requestResult;
	}

	bool CMockVisionClient::RequestDeviceCheckAsync(const StringMap& params)
	{
		std::lock_guard<std::mutex> lg(m_mutex);
		m_lastRequestParams = params;
		return m_requestResult;
	}

	bool CMockVisionClient::RequestLightAsync(const StringMap& params)
	{
		std::lock_guard<std::mutex> lg(m_mutex);
		m_lastRequestParams = params;
		return m_requestResult;
	}

	CMockVisionClient::DataMap CMockVisionClient::GetLatestData(VisionCommand type) const
	{
		std::lock_guard<std::mutex> lg(m_mutex);
		int key = type;
		std::map<int, StringMap>::const_iterator it = m_latestData.find(key);
		if (it != m_latestData.end()) return it->second;
		return DataMap();
	}

	void CMockVisionClient::ClearLatestData(VisionCommand type)
	{
		std::lock_guard<std::mutex> lg(m_mutex);
		m_latestData.erase(type);
		m_receivedFlags[type] = false;
	}

	bool CMockVisionClient::IsValid(VisionCommand type) const
	{
		return true;
	}

	bool CMockVisionClient::HasReceived(VisionCommand type) const
	{
		std::lock_guard<std::mutex> lg(m_mutex);
		int key = type;
		std::map<int, bool>::const_iterator it = m_receivedFlags.find(key);
		return (it != m_receivedFlags.end()) && it->second;
	}

	void CMockVisionClient::InitializeRecvThread()
	{
	}

	void CMockVisionClient::OnSetCok(ByteArray body)
	{
		std::lock_guard<std::mutex> lg(m_mutex);
		StringMap m;
		m["body"] = BodyToString(body);
		m_latestData[SetCok] = m;
		m_receivedFlags[SetCok] = true;
	}

	void CMockVisionClient::OnInspReady(ByteArray body)
	{
		std::lock_guard<std::mutex> lg(m_mutex);
		StringMap m;
		m["body"] = BodyToString(body);
		m_latestData[InspReady] = m;
		m_receivedFlags[InspReady] = true;
	}

	void CMockVisionClient::OnMeasure(ByteArray body)
	{
		std::lock_guard<std::mutex> lg(m_mutex);
		StringMap m;
		m["body"] = BodyToString(body);
		m_latestData[Measure] = m;
		m_receivedFlags[Measure] = true;
	}

	void CMockVisionClient::OnDeviceCheck(ByteArray body)
	{
		std::lock_guard<std::mutex> lg(m_mutex);
		StringMap m;
		m["body"] = BodyToString(body);
		m_latestData[DeviceCheck] = m;
		m_receivedFlags[DeviceCheck] = true;
	}

	void CMockVisionClient::OnLight(ByteArray body)
	{
		std::lock_guard<std::mutex> lg(m_mutex);
		StringMap m;
		m["body"] = BodyToString(body);
		m_latestData[Light] = m;
		m_receivedFlags[Light] = true;
	}

	void CMockVisionClient::SetRequestResult(bool ok)
	{
		std::lock_guard<std::mutex> lg(m_mutex);
		m_requestResult = ok;
	}

	StringMap CMockVisionClient::GetLastRequestParams() const
	{
		std::lock_guard<std::mutex> lg(m_mutex);
		return m_lastRequestParams;
	}

	std::string CMockVisionClient::BodyToString(const ByteArray& b)
	{
		if (b.empty()) return std::string();
		return std::string(reinterpret_cast<const char*>(b.data()), b.size());
	}

} // namespace VMF
