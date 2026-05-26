#pragma once

#include "IActuator.h"
#include "Actuators/Load1Parts.h"

namespace AutoThread
{
    class AdapterLoad1 : public VMF::IActuator
    {
        Load1Parts* m_parts;
    public:
        explicit AdapterLoad1(Load1Parts* parts);
        ~AdapterLoad1() override;

        VMF::ActError RailOpen();
        VMF::ActError RailClose();
        VMF::ActError IsRailOpen();
        VMF::ActError IsRailClose();

        VMF::ActError BufferForward();
        VMF::ActError BufferBackward();
        VMF::ActError IsBufferForward();
        VMF::ActError IsBufferBackward();

        VMF::ActError PusherForward();
        VMF::ActError PusherBackward();
        VMF::ActError IsPusherForward();
        VMF::ActError IsPusherBackward();

        VMF::ActError PreciserUp();
        VMF::ActError PreciserDown();
        VMF::ActError IsPreciserUp();
        VMF::ActError IsPreciserDown();

        VMF::ActError Clamp(int index);
        VMF::ActError Release(int index);
        VMF::ActError IsClamp(int index);
        VMF::ActError IsRelease(int index);

        VMF::ActError Vacuum(int index);
        VMF::ActError Blow(int index);
        VMF::ActError IsVacuum(int index);
        VMF::ActError IsBlow(int index);

    private:
        Load1Parts* GetParts();
    };
}
