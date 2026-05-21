#pragma once

#include "NonBlockingTaskBase.h"

namespace VMF_Load1
{
    namespace Task
    {
        class CPickPlaceTask : public VMF::NonBlockingTaskBase
        {
            enum Substep
            {
                MoveSafeZ = 0,
                MovePickPositionXY,
                MovePickPositionZ,
                ClampPick,
                MoveSafeZAfterPick,
                MovePlacePositionXY,
                MovePlacePositionZ,
                ReleasePlace,
                MoveSafeZAfterPlace,
                Complete
            };

            VMF::TaskResult HandleMoveSafeZ(VMF::Context& ctx, VMF::IActuator* actuator);
            VMF::TaskResult HandleMovePickPositionXY(VMF::Context& ctx, VMF::IActuator* actuator);
            VMF::TaskResult HandleMovePickPositionZ(VMF::Context& ctx, VMF::IActuator* actuator);
            VMF::TaskResult HandleClampPick(VMF::Context& ctx, VMF::IActuator* actuator);
            VMF::TaskResult HandleMoveSafeZAfterPick(VMF::Context& ctx, VMF::IActuator* actuator);
            VMF::TaskResult HandleMovePlacePositionXY(VMF::Context& ctx, VMF::IActuator* actuator);
            VMF::TaskResult HandleMovePlacePositionZ(VMF::Context& ctx, VMF::IActuator* actuator);
            VMF::TaskResult HandleReleasePlace(VMF::Context& ctx, VMF::IActuator* actuator);
            VMF::TaskResult HandleMoveSafeZAfterPlace(VMF::Context& ctx, VMF::IActuator* actuator);
            VMF::TaskResult HandleComplete(VMF::Context& ctx, VMF::IActuator* actuator);

        public:
            CPickPlaceTask();
            ~CPickPlaceTask() override;

            std::string GetName() const override
            {
                return "Task_PickPlace";
            }

        protected:
            void OnInitialize(VMF::Context& ctx) override;
            VMF::TaskResult OnPoll(VMF::Context& ctx, VMF::IActuator* actuator) override;

        private:
            std::vector<double> m_pickPosition;
            std::vector<double> m_placePosition;
            double m_safeZ;
            double m_pickZ;
            double m_placeZ;
            long m_moveTimeoutMs;
        };
    }
}
