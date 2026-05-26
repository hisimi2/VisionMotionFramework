#include "stdafx.h"
#include "CPickPlaceTaskEx.h"
#include "AdapterLoad1.h"

namespace AutoThread
{
    namespace Task
    {
        CPickPlaceTaskEx::CPickPlaceTaskEx()
            : m_safeZ(0.0)
            , m_pickZ(0.0)
            , m_placeZ(0.0)
            , m_moveTimeoutMs(3000)
            , m_clampIndex(0)
            , m_vacuumIndex(0)
        {
        }

        CPickPlaceTaskEx::~CPickPlaceTaskEx()
        {
        }

        void CPickPlaceTaskEx::OnInitialize(VMF::Context& ctx)
        {
            m_safeZ = ctx.GetSeqParamAs<double>("SafeZ", 0.0);
            m_pickZ = ctx.GetSeqParamAs<double>("PickZ", 0.0);
            m_placeZ = ctx.GetSeqParamAs<double>("PlaceZ", 0.0);
            m_moveTimeoutMs = ctx.GetSeqParamAs<long>("MoveTimeoutMs", 3000);
            m_clampIndex = ctx.GetSeqParamAs<int>("ClampIndex", 0);
            m_vacuumIndex = ctx.GetSeqParamAs<int>("VacuumIndex", 0);

            m_pickPosition.clear();
            m_pickPosition.push_back(ctx.GetSeqParamAs<double>("PickX", 0.0));
            m_pickPosition.push_back(ctx.GetSeqParamAs<double>("PickY", 0.0));

            m_placePosition.clear();
            m_placePosition.push_back(ctx.GetSeqParamAs<double>("PlaceX", 0.0));
            m_placePosition.push_back(ctx.GetSeqParamAs<double>("PlaceY", 0.0));

            EnterState(MoveSafeZ);
        }

        VMF::TaskResult CPickPlaceTaskEx::OnPoll(VMF::Context& ctx, VMF::IActuator* actuator)
        {
            switch (GetState())
            {
            case MoveSafeZ:           return HandleMoveSafeZ(ctx, actuator);
            case RailOpen:            return HandleRailOpen(ctx, actuator);
            case MovePickPositionXY:  return HandleMovePickPositionXY(ctx, actuator);
            case PreciserDown:        return HandlePreciserDown(ctx, actuator);
            case MovePickPositionZ:   return HandleMovePickPositionZ(ctx, actuator);
            case ClampPick:           return HandleClampPick(ctx, actuator);
            case VacuumOn:            return HandleVacuumOn(ctx, actuator);
            case MoveSafeZAfterPick:  return HandleMoveSafeZAfterPick(ctx, actuator);
            case MovePlacePositionXY: return HandleMovePlacePositionXY(ctx, actuator);
            case MovePlacePositionZ:  return HandleMovePlacePositionZ(ctx, actuator);
            case ReleasePlace:        return HandleReleasePlace(ctx, actuator);
            case BlowOn:              return HandleBlowOn(ctx, actuator);
            case MoveSafeZAfterPlace: return HandleMoveSafeZAfterPlace(ctx, actuator);
            case Complete:            return HandleComplete(ctx, actuator);
            default:                  return VMF::TR_ERROR;
            }
        }

        VMF::TaskResult CPickPlaceTaskEx::HandleMoveSafeZ(VMF::Context& ctx, VMF::IActuator* actuator)
        {
            if (!actuator)
                return SetErrorAndReturn(ctx, "PickPlaceEx: actuator is null.");

            if (actuator->MoveZ(m_safeZ) != VMF::ActOk)
                return SetErrorAndReturn(ctx, "PickPlaceEx: MoveSafeZ failed.");

            EnterStateWithTimeout(RailOpen, m_moveTimeoutMs);
            return VMF::TR_KEEP;
        }

        VMF::TaskResult CPickPlaceTaskEx::HandleRailOpen(VMF::Context& ctx, VMF::IActuator* actuator)
        {
            AdapterLoad1* ex = dynamic_cast<AdapterLoad1*>(actuator);
            if (!ex)
                return SetErrorAndReturn(ctx, "PickPlaceEx: actuator is not AdapterLoad1.");

            if (ex->IsRailOpen() != VMF::ActOk)
            {
                if (ex->RailOpen() != VMF::ActOk)
                    return SetErrorAndReturn(ctx, "PickPlaceEx: RailOpen failed.");
            }

            EnterState(MovePickPositionXY);
            return VMF::TR_KEEP;
        }

        VMF::TaskResult CPickPlaceTaskEx::HandleMovePickPositionXY(VMF::Context& ctx, VMF::IActuator* actuator)
        {
            if (!actuator)
                return SetErrorAndReturn(ctx, "PickPlaceEx: actuator is null.");

            if (actuator->Move(m_pickPosition, VMF::Narrow) != VMF::ActOk)
                return SetErrorAndReturn(ctx, "PickPlaceEx: MovePickPositionXY failed.");

            EnterStateWithTimeout(PreciserDown, m_moveTimeoutMs);
            return VMF::TR_KEEP;
        }

        VMF::TaskResult CPickPlaceTaskEx::HandlePreciserDown(VMF::Context& ctx, VMF::IActuator* actuator)
        {
            AdapterLoad1* ex = dynamic_cast<AdapterLoad1*>(actuator);
            if (!ex)
                return SetErrorAndReturn(ctx, "PickPlaceEx: actuator is not AdapterLoad1.");

            if (ex->PreciserDown() != VMF::ActOk)
                return SetErrorAndReturn(ctx, "PickPlaceEx: PreciserDown failed.");

            EnterStateWithTimeout(MovePickPositionZ, m_moveTimeoutMs);
            return VMF::TR_KEEP;
        }

        VMF::TaskResult CPickPlaceTaskEx::HandleMovePickPositionZ(VMF::Context& ctx, VMF::IActuator* actuator)
        {
            if (!actuator)
                return SetErrorAndReturn(ctx, "PickPlaceEx: actuator is null.");

            if (actuator->MoveZ(m_pickZ) != VMF::ActOk)
                return SetErrorAndReturn(ctx, "PickPlaceEx: MovePickPositionZ failed.");

            EnterStateWithTimeout(ClampPick, m_moveTimeoutMs);
            return VMF::TR_KEEP;
        }

        VMF::TaskResult CPickPlaceTaskEx::HandleClampPick(VMF::Context& ctx, VMF::IActuator* actuator)
        {
            AdapterLoad1* ex = dynamic_cast<AdapterLoad1*>(actuator);
            if (!ex)
                return SetErrorAndReturn(ctx, "PickPlaceEx: actuator is not AdapterLoad1.");

            if (ex->Clamp(m_clampIndex) != VMF::ActOk)
                return SetErrorAndReturn(ctx, "PickPlaceEx: ClampPick failed.");

            EnterState(VacuumOn);
            return VMF::TR_KEEP;
        }

        VMF::TaskResult CPickPlaceTaskEx::HandleVacuumOn(VMF::Context& ctx, VMF::IActuator* actuator)
        {
            AdapterLoad1* ex = dynamic_cast<AdapterLoad1*>(actuator);
            if (!ex)
                return SetErrorAndReturn(ctx, "PickPlaceEx: actuator is not AdapterLoad1.");

            if (ex->Vacuum(m_vacuumIndex) != VMF::ActOk)
                return SetErrorAndReturn(ctx, "PickPlaceEx: VacuumOn failed.");

            EnterState(MoveSafeZAfterPick);
            return VMF::TR_KEEP;
        }

        VMF::TaskResult CPickPlaceTaskEx::HandleMoveSafeZAfterPick(VMF::Context& ctx, VMF::IActuator* actuator)
        {
            if (!actuator)
                return SetErrorAndReturn(ctx, "PickPlaceEx: actuator is null.");

            if (actuator->MoveZ(m_safeZ) != VMF::ActOk)
                return SetErrorAndReturn(ctx, "PickPlaceEx: MoveSafeZAfterPick failed.");

            EnterState(MovePlacePositionXY);
            return VMF::TR_KEEP;
        }

        VMF::TaskResult CPickPlaceTaskEx::HandleMovePlacePositionXY(VMF::Context& ctx, VMF::IActuator* actuator)
        {
            if (!actuator)
                return SetErrorAndReturn(ctx, "PickPlaceEx: actuator is null.");

            if (actuator->Move(m_placePosition, VMF::Narrow) != VMF::ActOk)
                return SetErrorAndReturn(ctx, "PickPlaceEx: MovePlacePositionXY failed.");

            EnterState(MovePlacePositionZ);
            return VMF::TR_KEEP;
        }

        VMF::TaskResult CPickPlaceTaskEx::HandleMovePlacePositionZ(VMF::Context& ctx, VMF::IActuator* actuator)
        {
            if (!actuator)
                return SetErrorAndReturn(ctx, "PickPlaceEx: actuator is null.");

            if (actuator->MoveZ(m_placeZ) != VMF::ActOk)
                return SetErrorAndReturn(ctx, "PickPlaceEx: MovePlacePositionZ failed.");

            EnterState(ReleasePlace);
            return VMF::TR_KEEP;
        }

        VMF::TaskResult CPickPlaceTaskEx::HandleReleasePlace(VMF::Context& ctx, VMF::IActuator* actuator)
        {
            AdapterLoad1* ex = dynamic_cast<AdapterLoad1*>(actuator);
            if (!ex)
                return SetErrorAndReturn(ctx, "PickPlaceEx: actuator is not AdapterLoad1.");

            if (ex->Release(m_clampIndex) != VMF::ActOk)
                return SetErrorAndReturn(ctx, "PickPlaceEx: ReleasePlace failed.");

            EnterState(BlowOn);
            return VMF::TR_KEEP;
        }

        VMF::TaskResult CPickPlaceTaskEx::HandleBlowOn(VMF::Context& ctx, VMF::IActuator* actuator)
        {
            AdapterLoad1* ex = dynamic_cast<AdapterLoad1*>(actuator);
            if (!ex)
                return SetErrorAndReturn(ctx, "PickPlaceEx: actuator is not AdapterLoad1.");

            if (ex->Blow(m_vacuumIndex) != VMF::ActOk)
                return SetErrorAndReturn(ctx, "PickPlaceEx: BlowOn failed.");

            EnterState(MoveSafeZAfterPlace);
            return VMF::TR_KEEP;
        }

        VMF::TaskResult CPickPlaceTaskEx::HandleMoveSafeZAfterPlace(VMF::Context& ctx, VMF::IActuator* actuator)
        {
            if (!actuator)
                return SetErrorAndReturn(ctx, "PickPlaceEx: actuator is null.");

            if (actuator->MoveZ(m_safeZ) != VMF::ActOk)
                return SetErrorAndReturn(ctx, "PickPlaceEx: MoveSafeZAfterPlace failed.");

            EnterState(Complete);
            return VMF::TR_KEEP;
        }

        VMF::TaskResult CPickPlaceTaskEx::HandleComplete(VMF::Context& ctx, VMF::IActuator* actuator)
        {
            (void)ctx;
            (void)actuator;
            return VMF::TR_NEXT;
        }
    }
}
