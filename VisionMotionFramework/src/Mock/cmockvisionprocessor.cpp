#include "stdafx.h"
#include "Mock/CMockVisionProcessor.h"
#include "Controller.h"

namespace VMF
{
    CMockVisionProcessor::CMockVisionProcessor()
        : m_connected(false)
        , m_resultSink(nullptr)
    {
        // Initialize default command results to true (success)
        for (int i = SetCok; i <= Light; ++i)
        {
            VisionCommand cmd = static_cast<VisionCommand>(i);
            m_commandResults[cmd] = true;
            m_callCounts[cmd] = 0;
            m_hasReceived[cmd] = false;
        }
    }

    CMockVisionProcessor::~CMockVisionProcessor()
    {
    }

    VC::Status CMockVisionProcessor::Initialize(const VisionConnectionConfig& /*config*/)
    {
        std::lock_guard<std::mutex> lg(m_mutex);
        m_connected = true;
        return VC::VisionOK;
    }

    VC::Status CMockVisionProcessor::InitializeWithSharedController(
        std::shared_ptr<VC::Controller> sharedCtrl,
        const VisionConnectionConfig& /*config*/)
    {
        std::lock_guard<std::mutex> lg(m_mutex);
        m_connected = true;
        return VC::VisionOK;
    }

    void CMockVisionProcessor::Disconnect()
    {
        std::lock_guard<std::mutex> lg(m_mutex);
        m_connected = false;
    }

    bool CMockVisionProcessor::IsConnected() const
    {
        std::lock_guard<std::mutex> lg(m_mutex);
        return m_connected;
    }

    bool CMockVisionProcessor::RequestSetCokAsync(const StringMap& params)
    {
        std::lock_guard<std::mutex> lg(m_mutex);
        m_lastSetCokParams = params;
        ++m_callCounts[SetCok];
        return m_commandResults[SetCok];
    }

    bool CMockVisionProcessor::RequestInspReadyAsync(const StringMap& params)
    {
        std::lock_guard<std::mutex> lg(m_mutex);
        m_lastInspReadyParams = params;
        ++m_callCounts[InspReady];
        return m_commandResults[InspReady];
    }

    bool CMockVisionProcessor::RequestMeasureAsync(const StringMap& params)
    {
        std::lock_guard<std::mutex> lg(m_mutex);
        m_lastMeasureParams = params;
        ++m_callCounts[Measure];
        return m_commandResults[Measure];
    }

    bool CMockVisionProcessor::RequestDeviceCheckAsync(const StringMap& params)
    {
        std::lock_guard<std::mutex> lg(m_mutex);
        m_lastDeviceCheckParams = params;
        ++m_callCounts[DeviceCheck];
        return m_commandResults[DeviceCheck];
    }

    bool CMockVisionProcessor::RequestLightAsync(const StringMap& params)
    {
        std::lock_guard<std::mutex> lg(m_mutex);
        m_lastLightParams = params;
        ++m_callCounts[Light];
        return m_commandResults[Light];
    }

    CMockVisionProcessor::DataMap CMockVisionProcessor::GetLatestData(VisionCommand type) const
    {
        std::lock_guard<std::mutex> lg(m_mutex);
        auto it = m_latestData.find(type);
        if (it != m_latestData.end())
            return it->second;
        return DataMap();
    }

    void CMockVisionProcessor::ClearLatestData(VisionCommand type)
    {
        std::lock_guard<std::mutex> lg(m_mutex);
        m_latestData.erase(type);
        m_hasReceived[type] = false;
    }

    bool CMockVisionProcessor::IsValid(VisionCommand type) const
    {
        // For testing purposes, we'll consider all commands valid
        return true;
    }

    bool CMockVisionProcessor::HasReceived(VisionCommand type) const
    {
        std::lock_guard<std::mutex> lg(m_mutex);
        auto it = m_hasReceived.find(type);
        if (it != m_hasReceived.end())
            return it->second;
        return false;
    }

    void CMockVisionProcessor::InitializeRecvThread()
    {
        // No-op for mock
    }

    void CMockVisionProcessor::OnSetCok(ByteArray body)
    {
        std::lock_guard<std::mutex> lg(m_mutex);
        StringMap data;
        if (!body.empty())
        {
            std::string bodyStr(reinterpret_cast<const char*>(body.data()), body.size());
            data["body"] = bodyStr;
        }
        m_latestData[SetCok] = data;
        m_hasReceived[SetCok] = true;
        
        // Notify result sink if set
        if (m_resultSink)
        {
            std::vector<std::string> results;
            for (const auto& pair : data)
            {
                results.push_back(pair.first + "=" + pair.second);
            }
            m_resultSink->NotifyVisionResult(static_cast<int>(SetCok), results);
        }
    }

    void CMockVisionProcessor::OnInspReady(ByteArray body)
    {
        std::lock_guard<std::mutex> lg(m_mutex);
        StringMap data;
        if (!body.empty())
        {
            std::string bodyStr(reinterpret_cast<const char*>(body.data()), body.size());
            data["body"] = bodyStr;
        }
        m_latestData[InspReady] = data;
        m_hasReceived[InspReady] = true;
        
        if (m_resultSink)
        {
            std::vector<std::string> results;
            for (const auto& pair : data)
            {
                results.push_back(pair.first + "=" + pair.second);
            }
            m_resultSink->NotifyVisionResult(static_cast<int>(InspReady), results);
        }
    }

    void CMockVisionProcessor::OnMeasure(ByteArray body)
    {
        std::lock_guard<std::mutex> lg(m_mutex);
        StringMap data;
        if (!body.empty())
        {
            std::string bodyStr(reinterpret_cast<const char*>(body.data()), body.size());
            data["body"] = bodyStr;
        }
        m_latestData[Measure] = data;
        m_hasReceived[Measure] = true;
        
        if (m_resultSink)
        {
            std::vector<std::string> results;
            for (const auto& pair : data)
            {
                results.push_back(pair.first + "=" + pair.second);
            }
            m_resultSink->NotifyVisionResult(static_cast<int>(Measure), results);
        }
    }

    void CMockVisionProcessor::OnDeviceCheck(ByteArray body)
    {
        std::lock_guard<std::mutex> lg(m_mutex);
        StringMap data;
        if (!body.empty())
        {
            std::string bodyStr(reinterpret_cast<const char*>(body.data()), body.size());
            data["body"] = bodyStr;
        }
        m_latestData[DeviceCheck] = data;
        m_hasReceived[DeviceCheck] = true;
        
        if (m_resultSink)
        {
            std::vector<std::string> results;
            for (const auto& pair : data)
            {
                results.push_back(pair.first + "=" + pair.second);
            }
            m_resultSink->NotifyVisionResult(static_cast<int>(DeviceCheck), results);
        }
    }

    void CMockVisionProcessor::OnLight(ByteArray body)
    {
        std::lock_guard<std::mutex> lg(m_mutex);
        StringMap data;
        if (!body.empty())
        {
            std::string bodyStr(reinterpret_cast<const char*>(body.data()), body.size());
            data["body"] = bodyStr;
        }
        m_latestData[Light] = data;
        m_hasReceived[Light] = true;
        
        if (m_resultSink)
        {
            std::vector<std::string> results;
            for (const auto& pair : data)
            {
                results.push_back(pair.first + "=" + pair.second);
            }
            m_resultSink->NotifyVisionResult(static_cast<int>(Light), results);
        }
    }

    void CMockVisionProcessor::SetResultSink(IResultSink* sink)
    {
        m_resultSink = sink;
    }

    void CMockVisionProcessor::StartProcessThread()
    {
        // No-op for mock
    }

    void CMockVisionProcessor::StopProcessThread()
    {
        // No-op for mock
    }

    // Helper methods for test control
    void CMockVisionProcessor::SetCommandResult(VisionCommand cmd, bool result)
    {
        std::lock_guard<std::mutex> lg(m_mutex);
        m_commandResults[cmd] = result;
    }

    void CMockVisionProcessor::SetLatestData(VisionCommand type, const DataMap& data)
    {
        std::lock_guard<std::mutex> lg(m_mutex);
        m_latestData[type] = data;
        m_hasReceived[type] = true;
    }

    void CMockVisionProcessor::ClearCommandHistory()
    {
        std::lock_guard<std::mutex> lg(m_mutex);
        m_lastSetCokParams.clear();
        m_lastInspReadyParams.clear();
        m_lastMeasureParams.clear();
        m_lastDeviceCheckParams.clear();
        m_lastLightParams.clear();
        
        for (auto& pair : m_callCounts)
        {
            pair.second = 0;
        }
        
        m_latestData.clear();
        m_hasReceived.clear();
        for (int i = SetCok; i <= Light; ++i)
        {
            VisionCommand cmd = static_cast<VisionCommand>(i);
            m_hasReceived[cmd] = false;
        }
    }

    void CMockVisionProcessor::SetConnectStatus(bool connected)
    {
        std::lock_guard<std::mutex> lg(m_mutex);
        m_connected = connected;
    }

    // Getters for test verification
    const StringMap& CMockVisionProcessor::GetLastRequestParams(VisionCommand cmd) const
    {
        switch (cmd)
        {
            case SetCok: return m_lastSetCokParams;
            case InspReady: return m_lastInspReadyParams;
            case Measure: return m_lastMeasureParams;
            case DeviceCheck: return m_lastDeviceCheckParams;
            case Light: return m_lastLightParams;
            default: 
            {
                static StringMap empty;
                return empty;
            }
        }
    }

    bool CMockVisionProcessor::GetCommandWasCalled(VisionCommand cmd) const
    {
        std::lock_guard<std::mutex> lg(m_mutex);
        auto it = m_callCounts.find(cmd);
        if (it != m_callCounts.end())
            return it->second > 0;
        return false;
    }

    int CMockVisionProcessor::GetCallCount(VisionCommand cmd) const
    {
        std::lock_guard<std::mutex> lg(m_mutex);
        auto it = m_callCounts.find(cmd);
        if (it != m_callCounts.end())
            return it->second;
        return 0;
    }

} // namespace VMF