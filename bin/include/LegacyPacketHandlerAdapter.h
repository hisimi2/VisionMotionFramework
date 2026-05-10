#pragma once

#include "VisionMsgDispatcher.h"
#include <functional>

namespace VisionComm
{
    // Helpers to adapt legacy handlers that expect const ByteArray& into the new PacketHandler

    // Use when you have a callable that accepts 'const ByteArray&'
    template<typename Callable>
    PacketHandler MakePacketHandlerForLegacy(Callable cb)
    {
        std::function<void(const ByteArray&)> f = cb; // ensure copyable callable
        return [f](int s, int fcode, ByteArray&& body, int serverIndex) mutable {
            // pass as const reference to legacy callable
            f(body);
        };
    }

    // Use when you have a callable that accepts ByteArray by-value and you want to move into it
    template<typename Callable>
    PacketHandler MakePacketHandlerMove(Callable cb)
    {
        std::function<void(ByteArray)> f = cb;
        return [f](int s, int fcode, ByteArray&& body, int serverIndex) mutable {
            f(std::move(body));
        };
    }

} // namespace VisionCommm

