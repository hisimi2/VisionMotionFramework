#include "stdafx.h"
#include "Mock/CMockVisionEventHandler.h"

namespace VMF
{
	CMockVisionEventHandler::CMockVisionEventHandler()
		: m_connected(false)
		, m_requestResult(true)
	{
	}

	CMockVisionEventHandler::~CMockVisionEventHandler()
	{
	}

	VC::Status CMockVisionEventHandler::Initialize(const VisionConnectionConfig& /*config*/)
	{
		std::lock_guard<std::mutex> lg(m_mutex);
		m_connected = true;
		return VC::VisionOK;
	}

	VC::Status CMockVisionEventHandler::InitializeWithSharedController(
		std::shared_ptr<VC::Controller> sharedCtrl,
		const VisionConnectionConfig& config)
	{
		std::lock_guard<std::mutex> lg(m_mutex);
		m_sharedCtrl = sharedCtrl;
		m_connected = true;
		return VC::VisionOK;
	}

	void CMockVisionEventHandler::Disconnect()
	{
		std::lock_guard<std::mutex> lg(m_mutex);
		m_connected = false;
	}

	bool CMockVisionEventHandler::IsConnected() const
	{
		std::lock_guard<std::mutex> lg(m_mutex);
		return m_connected;
	}

	bool CMockVisionEventHandler::RequestSetCokAsync(const StringMap& params)
	{
		std::lock_guard<std::mutex> lg(m_mutex);
		m_lastRequestParams = params;
		return m_requestResult;
	}

	bool CMockVisionEventHandler::RequestInspReadyAsync(const StringMap& params)
	{
		std::lock_guard<std::mutex> lg(m_mutex);
		m_lastRequestParams = params;
		return m_requestResult;
	}

	bool CMockVisionEventHandler::RequestMeasureAsync(const StringMap& params)
	{
		std::lock_guard<std::mutex> lg(m_mutex);
		m_lastRequestParams = params;
		return m_requestResult;
	}

	bool CMockVisionEventHandler::RequestDeviceCheckAsync(const StringMap& params)
	{
		std::lock_guard<std::mutex> lg(m_mutex);
		m_lastRequestParams = params;
		return m_requestResult;
	}

	bool CMockVisionEventHandler::RequestLightAsync(const StringMap& params)
	{
		std::lock_guard<std::mutex> lg(m_mutex);
		m_lastRequestParams = params;
		return m_requestResult;
	}

	CMockVisionEventHandler::DataMap CMockVisionEventHandler::GetLatestData(VisionCommand type) const
	{
		std::lock_guard<std::mutex> lg(m_mutex);
		int key = type;
		std::map<int, StringMap>::const_iterator it = m_latestData.find(key);
		if (it != m_latestData.end()) return it->second;
		return DataMap();
	}

	void CMockVisionEventHandler::ClearLatestData(VisionCommand type)
	{
		std::lock_guard<std::mutex> lg(m_mutex);
		m_latestData.erase(type);
		m_receivedFlags[type] = false;
	}

	bool CMockVisionEventHandler::IsValid(VisionCommand type) const
	{
		return true;
	}

	bool CMockVisionEventHandler::HasReceived(VisionCommand type) const
	{
		std::lock_guard<std::mutex> lg(m_mutex);
		int key = type;
		std::map<int, bool>::const_iterator it = m_receivedFlags.find(key);
		return (it != m_receivedFlags.end()) && it->second;
	}

	void CMockVisionEventHandler::InitializeRecvThread()
	{
	}

	void CMockVisionEventHandler::OnSetCok(ByteArray body)
	{
		std::lock_guard<std::mutex> lg(m_mutex);
		StringMap m;
		m["body"] = BodyToString(body);
		m_latestData[SetCok] = m;
		m_receivedFlags[SetCok] = true;
	}

	void CMockVisionEventHandler::OnInspReady(ByteArray body)
	{
		std::lock_guard<std::mutex> lg(m_mutex);
		StringMap m;
		m["body"] = BodyToString(body);
		m_latestData[InspReady] = m;
		m_receivedFlags[InspReady] = true;
	}

	void CMockVisionEventHandler::OnMeasure(ByteArray body)
	{
		std::lock_guard<std::mutex> lg(m_mutex);
		StringMap m;
		m["body"] = BodyToString(body);
		m_latestData[Measure] = m;
		m_receivedFlags[Measure] = true;
	}

	void CMockVisionEventHandler::OnDeviceCheck(ByteArray body)
	{
		std::lock_guard<std::mutex> lg(m_mutex);
		StringMap m;
		m["body"] = BodyToString(body);
		m_latestData[DeviceCheck] = m;
		m_receivedFlags[DeviceCheck] = true;
	}

	void CMockVisionEventHandler::OnLight(ByteArray body)
	{
		std::lock_guard<std::mutex> lg(m_mutex);
		StringMap m;
		m["body"] = BodyToString(body);
		m_latestData[Light] = m;
		m_receivedFlags[Light] = true;
	}

	void CMockVisionEventHandler::SetRequestResult(bool ok)
	{
		std::lock_guard<std::mutex> lg(m_mutex);
		m_requestResult = ok;
	}

	StringMap CMockVisionEventHandler::GetLastRequestParams() const
	{
		std::lock_guard<std::mutex> lg(m_mutex);
		return m_lastRequestParams;
	}

	std::string CMockVisionEventHandler::BodyToString(const ByteArray& b)
	{
		if (b.empty()) return std::string();
		return std::string(reinterpret_cast<const char*>(b.data()), b.size());
	}

} // namespace VMF
