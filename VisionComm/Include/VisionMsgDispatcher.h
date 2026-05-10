#pragma once

#include "IVisionPacket.h"
#include "SECSIPacket.h"
#include "VisionComAPI.h"
#include "VisionProtocol.h"

#include <vector>
#include <cstdint>
#include <memory>
#include <functional>

namespace VisionComm { class IScheduler; }

namespace VisionComm
{
 using ByteArray = std::vector<uint8_t>;
 // PacketHandler now requires an rvalue-reference for body to enforce move-only semantics
 using PacketHandler = std::function<void(int /*S*/, int /*F*/, ByteArray&& /*body*/, int /*serverIndex*/)>;

 class VISION_COM_API VisionMsgDispatcher
 {
 public:
 explicit VisionMsgDispatcher();
 ~VisionMsgDispatcher();

 void RegisterHandler(int s, int f, PacketHandler handler);
 void RegisterHandler(const VisionProtocolId& protocolId, PacketHandler handler);
 void UnregisterHandler(int s, int f);
 void UnregisterHandler(const VisionProtocolId& protocolId);
 void OnReceive(int s, int f, ByteArray&& body, int serverIndex);
 bool HasHandler(int s, int f);
 bool HasHandler(const VisionProtocolId& protocolId);

 void Dispatch(const SECSPacketHeader& header, ByteArray&& body);
 void SetScheduler(std::shared_ptr<IScheduler> scheduler);

 private:
 struct Impl;
 std::unique_ptr<Impl> m_pImpl;
 };

} // namespace VisionCommm

