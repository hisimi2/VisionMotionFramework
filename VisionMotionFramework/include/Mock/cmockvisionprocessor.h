/*
 * This is an auto-generated file.
 * DO NOT MODIFY DIRECTLY.
 */

#pragma once

#include "VisionProcessorBase.h"
#include "Mock/CMockVisionEventHandler.h"

namespace VMF
{
    /**
     * Mock implementation of IVisionProcessor for unit testing.
     * This mock provides controlled responses for all IVisionProcessor methods.
     */
    class VMF_API CMockVisionProcessor : public IVisionProcessor
    {
    public:
        CMockVisionProcessor();
        virtual ~CMockVisionProcessor();

        // IVisionProcessor interface
        VC::Status Initialize(const VisionConnectionConfig& config) override;
        VC::Status InitializeWithSharedController(std::shared_ptr<VC::Controller> sharedCtrl,
                                               const VisionConnectionConfig& config) override;
        void Disconnect() override;
        bool IsConnected() const override;

        // Command execution methods
        bool RequestSetCokAsync(const StringMap& params) override;
        bool RequestInspReadyAsync(const StringMap& params) override;
        bool RequestMeasureAsync(const StringMap& params) override;
        bool RequestDeviceCheckAsync(const StringMap& params) override;
        bool RequestLightAsync(const StringMap& params) override;

        // Data management methods
        DataMap GetLatestData(VisionCommand type) const override;
        void ClearLatestData(VisionCommand type) override;
        bool IsValid(VisionCommand type) const override;
        bool HasReceived(VisionCommand type) const override;

        // Thread management methods
        void InitializeRecvThread() override;
        void OnSetCok(ByteArray body) override;
        void OnInspReady(ByteArray body) override;
        void OnMeasure(ByteArray body) override;
        void OnDeviceCheck(ByteArray body) override;
        void OnLight(ByteArray body) override;

// Configuration methods (from VisionProcessorBase - not virtual in interface)
        void SetResultSink(IResultSink* sink);
        void StartProcessThread();
        void StopProcessThread();

        // Helper methods for test control
        void SetCommandResult(VisionCommand cmd, bool result);
        void SetLatestData(VisionCommand type, const DataMap& data);
        void ClearCommandHistory();
        void SetConnectStatus(bool connected);

        // Getters for test verification
        const StringMap& GetLastRequestParams(VisionCommand cmd) const;
        bool GetCommandWasCalled(VisionCommand cmd) const;
        int GetCallCount(VisionCommand cmd) const;

    private:
        // Internal state tracking
        std::map<VisionCommand, bool> m_commandResults;
        std::map<VisionCommand, DataMap> m_latestData;
        std::map<VisionCommand, bool> m_hasReceived;
        std::map<VisionCommand, int> m_callCounts;
        IResultSink* m_resultSink;
        bool m_connected;
mutable std::mutex m_mutex;
        
        // For tracking last request parameters
        StringMap m_lastSetCokParams;
        StringMap m_lastInspReadyParams;
        StringMap m_lastMeasureParams;
        StringMap m_lastDeviceCheckParams;
        StringMap m_lastLightParams;
    };
}