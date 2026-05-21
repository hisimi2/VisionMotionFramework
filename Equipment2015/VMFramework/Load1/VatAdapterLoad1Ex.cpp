#include "StdAfx.h"
#include "VatAdapterLoad1Ex.h"

namespace VMF_Load1
{
    VatAdapterLoad1Ex::VatAdapterLoad1Ex(Load1Parts* parts)
        : VatAdapterLoad1(parts)
    {
    }

    VatAdapterLoad1Ex::~VatAdapterLoad1Ex()
    {
    }

    Load1Parts* VatAdapterLoad1Ex::GetParts()
    {
        // base class의 m_parts가 private이면 protected로 바꾸거나
        // base에 getter를 추가하는 것이 더 좋음.
        // 여기서는 설명용으로 reinterpret_cast 없이 직접 접근 가능한 구조를 전제하지 않으므로
        // 실제 적용 시 VatAdapterLoad1에 protected getter 추가 권장.
        return nullptr;
    }

    VMF::ActError VatAdapterLoad1Ex::RailOpen()
    {
        Load1Parts* parts = GetParts();
        if (!parts) return VMF::ActNoParts;
        return parts->LoadRail.open(true) ? VMF::ActOk : VMF::ActFail;
    }

    VMF::ActError VatAdapterLoad1Ex::RailClose()
    {
        Load1Parts* parts = GetParts();
        if (!parts) return VMF::ActNoParts;
        return parts->LoadRail.close(true) ? VMF::ActOk : VMF::ActFail;
    }

    VMF::ActError VatAdapterLoad1Ex::IsRailOpen()
    {
        Load1Parts* parts = GetParts();
        if (!parts) return VMF::ActNoParts;
        return parts->LoadRail.isOpen() ? VMF::ActOk : VMF::ActWait;
    }

    VMF::ActError VatAdapterLoad1Ex::IsRailClose()
    {
        Load1Parts* parts = GetParts();
        if (!parts) return VMF::ActNoParts;
        return parts->LoadRail.isClose() ? VMF::ActOk : VMF::ActWait;
    }

    VMF::ActError VatAdapterLoad1Ex::BufferForward()
    {
        Load1Parts* parts = GetParts();
        if (!parts) return VMF::ActNoParts;
        return parts->LoadBuffer.forward(true) ? VMF::ActOk : VMF::ActFail;
    }

    VMF::ActError VatAdapterLoad1Ex::BufferBackward()
    {
        Load1Parts* parts = GetParts();
        if (!parts) return VMF::ActNoParts;
        return parts->LoadBuffer.backward(true) ? VMF::ActOk : VMF::ActFail;
    }

    VMF::ActError VatAdapterLoad1Ex::IsBufferForward()
    {
        Load1Parts* parts = GetParts();
        if (!parts) return VMF::ActNoParts;
        return parts->LoadBuffer.isForward() ? VMF::ActOk : VMF::ActWait;
    }

    VMF::ActError VatAdapterLoad1Ex::IsBufferBackward()
    {
        Load1Parts* parts = GetParts();
        if (!parts) return VMF::ActNoParts;
        return parts->LoadBuffer.isBackward() ? VMF::ActOk : VMF::ActWait;
    }

    VMF::ActError VatAdapterLoad1Ex::PusherForward()
    {
        Load1Parts* parts = GetParts();
        if (!parts) return VMF::ActNoParts;
        return parts->LoadPusher.forward(true) ? VMF::ActOk : VMF::ActFail;
    }

    VMF::ActError VatAdapterLoad1Ex::PusherBackward()
    {
        Load1Parts* parts = GetParts();
        if (!parts) return VMF::ActNoParts;
        return parts->LoadPusher.backward(true) ? VMF::ActOk : VMF::ActFail;
    }

    VMF::ActError VatAdapterLoad1Ex::IsPusherForward()
    {
        Load1Parts* parts = GetParts();
        if (!parts) return VMF::ActNoParts;
        return parts->LoadPusher.isForward() ? VMF::ActOk : VMF::ActWait;
    }

    VMF::ActError VatAdapterLoad1Ex::IsPusherBackward()
    {
        Load1Parts* parts = GetParts();
        if (!parts) return VMF::ActNoParts;
        return parts->LoadPusher.isBackward() ? VMF::ActOk : VMF::ActWait;
    }

    VMF::ActError VatAdapterLoad1Ex::PreciserUp()
    {
        Load1Parts* parts = GetParts();
        if (!parts) return VMF::ActNoParts;
        return parts->LoadPreciser.up(true) ? VMF::ActOk : VMF::ActFail;
    }

    VMF::ActError VatAdapterLoad1Ex::PreciserDown()
    {
        Load1Parts* parts = GetParts();
        if (!parts) return VMF::ActNoParts;
        return parts->LoadPreciser.down(true) ? VMF::ActOk : VMF::ActFail;
    }

    VMF::ActError VatAdapterLoad1Ex::IsPreciserUp()
    {
        Load1Parts* parts = GetParts();
        if (!parts) return VMF::ActNoParts;
        return parts->LoadPreciser.isUp() ? VMF::ActOk : VMF::ActWait;
    }

    VMF::ActError VatAdapterLoad1Ex::IsPreciserDown()
    {
        Load1Parts* parts = GetParts();
        if (!parts) return VMF::ActNoParts;
        return parts->LoadPreciser.isDown() ? VMF::ActOk : VMF::ActWait;
    }

    VMF::ActError VatAdapterLoad1Ex::Clamp(int index)
    {
        Load1Parts* parts = GetParts();
        if (!parts) return VMF::ActNoParts;
        if (index < 0 || index >= static_cast<int>(parts->TransferClamp.size())) return VMF::ActInvalidParam;
        return parts->TransferClamp[index].clamp(true) ? VMF::ActOk : VMF::ActFail;
    }

    VMF::ActError VatAdapterLoad1Ex::Release(int index)
    {
        Load1Parts* parts = GetParts();
        if (!parts) return VMF::ActNoParts;
        if (index < 0 || index >= static_cast<int>(parts->TransferClamp.size())) return VMF::ActInvalidParam;
        return parts->TransferClamp[index].release(true) ? VMF::ActOk : VMF::ActFail;
    }

    VMF::ActError VatAdapterLoad1Ex::IsClamp(int index)
    {
        Load1Parts* parts = GetParts();
        if (!parts) return VMF::ActNoParts;
        if (index < 0 || index >= static_cast<int>(parts->TransferClamp.size())) return VMF::ActInvalidParam;
        return parts->TransferClamp[index].isClamp() ? VMF::ActOk : VMF::ActWait;
    }

    VMF::ActError VatAdapterLoad1Ex::IsRelease(int index)
    {
        Load1Parts* parts = GetParts();
        if (!parts) return VMF::ActNoParts;
        if (index < 0 || index >= static_cast<int>(parts->TransferClamp.size())) return VMF::ActInvalidParam;
        return parts->TransferClamp[index].isRelease() ? VMF::ActOk : VMF::ActWait;
    }

    VMF::ActError VatAdapterLoad1Ex::Vacuum(int index)
    {
        // Load1Parts에 vacuum 계열 멤버가 없으므로 실제 적용 시 멤버 추가 필요
        (void)index;
        return VMF::ActNotReady;
    }

    VMF::ActError VatAdapterLoad1Ex::Blow(int index)
    {
        (void)index;
        return VMF::ActNotReady;
    }

    VMF::ActError VatAdapterLoad1Ex::IsVacuum(int index)
    {
        (void)index;
        return VMF::ActNotReady;
    }

    VMF::ActError VatAdapterLoad1Ex::IsBlow(int index)
    {
        (void)index;
        return VMF::ActNotReady;
    }
}
