#include "stdafx.h"
#include "CPickPlaceTask.h"

namespace VMF_Load1
{
    namespace Task
    {
        CPickPlaceTask::CPickPlaceTask()
            : m_safeZ(0.0)
            , m_pickZ(0.0)
            , m_placeZ(0.0)
            , m_moveTimeoutMs(3000)
        {
        }

        CPickPlaceTask::~CPickPlaceTask()
        {
        }

        void CPickPlaceTask::OnInitialize(VMF::Context& ctx)
        {
            m_safeZ = ctx.GetSeqParamAs<double>("SafeZ", 0.0);
            m_pickZ = ctx.GetSeqParamAs<double>("PickZ", 0.0);
            m_placeZ = ctx.GetSeqParamAs<double>("PlaceZ", 0.0);
            m_moveTimeoutMs = ctx.GetSeqParamAs<long>("MoveTimeoutMs", 3000);

            const double pickX = ctx.GetSeqParamAs<double>("PickX", 0.0);
            const double pickY = ctx.GetSeqParamAs<double>("PickY", 0.0);
            const double placeX = ctx.GetSeqParamAs<double>("PlaceX", 0.0);
            const double placeY = ctx.GetSeqParamAs<double>("PlaceY", 0.0);

            m_pickPosition.clear();
            m_pickPosition.push_back(pickX);
            m_pickPosition.push_back(pickY);

            m_placePosition.clear();
            m_placePosition.push_back(placeX);
            m_placePosition.push_back(placeY);

            EnterState(MoveSafeZ);
        }

        VMF::TaskResult CPickPlaceTask::OnPoll(VMF::Context& ctx, VMF::IActuator* actuator)
        {
            switch (GetState())
            {
            case MoveSafeZ:            return HandleMoveSafeZ(ctx, actuator);
            case MovePickPositionXY:   return HandleMovePickPositionXY(ctx, actuator);
            case MovePickPositionZ:    return HandleMovePickPositionZ(ctx, actuator);
            case ClampPick:            return HandleClampPick(ctx, actuator);
            case MoveSafeZAfterPick:   return HandleMoveSafeZAfterPick(ctx, actuator);
            case MovePlacePositionXY:  return HandleMovePlacePositionXY(ctx, actuator);
            case MovePlacePositionZ:   return HandleMovePlacePositionZ(ctx, actuator);
            case ReleasePlace:         return HandleReleasePlace(ctx, actuator);
            case MoveSafeZAfterPlace:  return HandleMoveSafeZAfterPlace(ctx, actuator);
            case Complete:             return HandleComplete(ctx, actuator);
            default:                   return VMF::TR_ERROR;
            }
        }

        VMF::TaskResult CPickPlaceTask::HandleMoveSafeZ(VMF::Context& ctx, VMF::IActuator* actuator)
        {
            if (!actuator)
                return SetErrorAndReturn(ctx, "PickPlace: actuator is null.");

            if (actuator->MoveZ(m_safeZ) != VMF::ActOk)
                return SetErrorAndReturn(ctx, "PickPlace: MoveSafeZ failed.");

            EnterStateWithTimeout(MovePickPositionXY, m_moveTimeoutMs);
            return VMF::TR_KEEP;
        }

        VMF::TaskResult CPickPlaceTask::HandleMovePickPositionXY(VMF::Context& ctx, VMF::IActuator* actuator)
        {
            if (!actuator)
                return SetErrorAndReturn(ctx, "PickPlace: actuator is null.");

            if (actuator->isMoveZ(m_safeZ) != VMF::ActOk)
            {
                if (IsDeadlineExpired())
                    return SetErrorAndReturn(ctx, "PickPlace: SafeZ timeout.");
                return VMF::TR_KEEP;
            }

            if (actuator->Move(m_pickPosition, VMF::Narrow) != VMF::ActOk)
                return SetErrorAndReturn(ctx, "PickPlace: MovePickPositionXY failed.");

            EnterStateWithTimeout(MovePickPositionZ, m_moveTimeoutMs);
            return VMF::TR_KEEP;
        }

        VMF::TaskResult CPickPlaceTask::HandleMovePickPositionZ(VMF::Context& ctx, VMF::IActuator* actuator)
        {
            if (!actuator)
                return SetErrorAndReturn(ctx, "PickPlace: actuator is null.");

            if (actuator->isMove(m_pickPosition, VMF::Narrow) != VMF::ActOk)
            {
                if (IsDeadlineExpired())
                    return SetErrorAndReturn(ctx, "PickPlace: Pick XY timeout.");
                return VMF::TR_KEEP;
            }

            if (actuator->MoveZ(m_pickZ) != VMF::ActOk)
                return SetErrorAndReturn(ctx, "PickPlace: MovePickPositionZ failed.");

            EnterStateWithTimeout(ClampPick, m_moveTimeoutMs);
            return VMF::TR_KEEP;
        }

        VMF::TaskResult CPickPlaceTask::HandleClampPick(VMF::Context& ctx, VMF::IActuator* actuator)
        {
            if (!actuator)
                return SetErrorAndReturn(ctx, "PickPlace: actuator is null.");

            if (actuator->isMoveZ(m_pickZ) != VMF::ActOk)
            {
                if (IsDeadlineExpired())
                    return SetErrorAndReturn(ctx, "PickPlace: Pick Z timeout.");
                return VMF::TR_KEEP;
            }

            EnterState(MoveSafeZAfterPick);
            return VMF::TR_KEEP;
        }

        VMF::TaskResult CPickPlaceTask::HandleMoveSafeZAfterPick(VMF::Context& ctx, VMF::IActuator* actuator)
        {
            if (!actuator)
                return SetErrorAndReturn(ctx, "PickPlace: actuator is null.");

            if (actuator->MoveZ(m_safeZ) != VMF::ActOk)
                return SetErrorAndReturn(ctx, "PickPlace: MoveSafeZAfterPick failed.");

            EnterStateWithTimeout(MovePlacePositionXY, m_moveTimeoutMs);
            return VMF::TR_KEEP;
        }

        VMF::TaskResult CPickPlaceTask::HandleMovePlacePositionXY(VMF::Context& ctx, VMF::IActuator* actuator)
        {
            if (!actuator)
                return SetErrorAndReturn(ctx, "PickPlace: actuator is null.");

            if (actuator->isMoveZ(m_safeZ) != VMF::ActOk)
            {
                if (IsDeadlineExpired())
                    return SetErrorAndReturn(ctx, "PickPlace: SafeZ after pick timeout.");
                return VMF::TR_KEEP;
            }

            if (actuator->Move(m_placePosition, VMF::Narrow) != VMF::ActOk)
                return SetErrorAndReturn(ctx, "PickPlace: MovePlacePositionXY failed.");

            EnterStateWithTimeout(MovePlacePositionZ, m_moveTimeoutMs);
            return VMF::TR_KEEP;
        }

        VMF::TaskResult CPickPlaceTask::HandleMovePlacePositionZ(VMF::Context& ctx, VMF::IActuator* actuator)
        {
            if (!actuator)
                return SetErrorAndReturn(ctx, "PickPlace: actuator is null.");

            if (actuator->isMove(m_placePosition, VMF::Narrow) != VMF::ActOk)
            {
                if (IsDeadlineExpired())
                    return SetErrorAndReturn(ctx, "PickPlace: Place XY timeout.");
                return VMF::TR_KEEP;
            }

            if (actuator->MoveZ(m_placeZ) != VMF::ActOk)
                return SetErrorAndReturn(ctx, "PickPlace: MovePlacePositionZ failed.");

            EnterStateWithTimeout(ReleasePlace, m_moveTimeoutMs);
            return VMF::TR_KEEP;
        }

        VMF::TaskResult CPickPlaceTask::HandleReleasePlace(VMF::Context& ctx, VMF::IActuator* actuator)
        {
            if (!actuator)
                return SetErrorAndReturn(ctx, "PickPlace: actuator is null.");

            if (actuator->isMoveZ(m_placeZ) != VMF::ActOk)
            {
                if (IsDeadlineExpired())
                    return SetErrorAndReturn(ctx, "PickPlace: Place Z timeout.");
                return VMF::TR_KEEP;
            }

            EnterState(MoveSafeZAfterPlace);
            return VMF::TR_KEEP;
        }

        VMF::TaskResult CPickPlaceTask::HandleMoveSafeZAfterPlace(VMF::Context& ctx, VMF::IActuator* actuator)
        {
            if (!actuator)
                return SetErrorAndReturn(ctx, "PickPlace: actuator is null.");

            if (actuator->MoveZ(m_safeZ) != VMF::ActOk)
                return SetErrorAndReturn(ctx, "PickPlace: MoveSafeZAfterPlace failed.");

            EnterStateWithTimeout(Complete, m_moveTimeoutMs);
            return VMF::TR_KEEP;
        }

        VMF::TaskResult CPickPlaceTask::HandleComplete(VMF::Context& ctx, VMF::IActuator* actuator)
        {
            if (!actuator)
                return SetErrorAndReturn(ctx, "PickPlace: actuator is null.");

            if (actuator->isMoveZ(m_safeZ) != VMF::ActOk)
            {
                if (IsDeadlineExpired())
                    return SetErrorAndReturn(ctx, "PickPlace: Final SafeZ timeout.");
                return VMF::TR_KEEP;
            }

            return VMF::TR_NEXT;
        }
    }
}
