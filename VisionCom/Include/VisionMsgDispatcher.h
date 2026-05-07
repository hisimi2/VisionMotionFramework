#pragma once

#include "IVisionPacket.h"
#include "SECSIPacket.h"
#include "VisionComAPI.h"
#include "VisionProtocol.h"

#include <vector>
#include <cstdint>
#include <memory>
#include <functional>

namespace VisionCom { class IScheduler; }

namespace VisionCom
{
 using PacketHandler = std::function<void(int /*S*/, int /*F*/, const std::vector<uint8_t>& /*body*/, int /*serverIndex*/)>;
 using ByteArray = std::vector<uint8_t>;

 class VISION_COM_API VisionMsgDispatcher
 {
 public:
 explicit VisionMsgDispatcher();
 ~VisionMsgDispatcher();

 void RegisterHandler(int s, int f, PacketHandler handler);
 void RegisterHandler(const VisionProtocolId& protocolId, PacketHandler handler);
 void UnregisterHandler(int s, int f);
 void UnregisterHandler(const VisionProtocolId& protocolId);
 void OnReceive(int s, int f, const std::vector<uint8_t>& body, int serverIndex);
 bool HasHandler(int s, int f);
 bool HasHandler(const VisionProtocolId& protocolId);

 void Dispatch(const SECSPacketHeader& header, const ByteArray& body);
 void SetScheduler(std::shared_ptr<IScheduler> scheduler);

 private:
 struct Impl;
 std::unique_ptr<Impl> m_pImpl;
 };

} // namespace VisionCom
