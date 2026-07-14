#include "pch.h"
#include "C6SideInspectionSequenceBuilder.h"
#include "..\Tasks\C6SideMoveToFacePositionTask.h"
#include "..\Tasks\C6SideInspectionTask.h"

namespace VMF_PLUGIN
{
    VMF::SequencePtr C6SideInspectionSequenceBuilder::BuildSequence(const std::string& sequenceName) 
    {
        VMF::SequencePtr seq(new VMF::Sequence(sequenceName));

        // ── 면 1: Left ────────────────────────────────────
        seq->AddTask(VMF::TaskPtr(new C6SideMoveToFacePositionTask())); 
        seq->AddTask(VMF::TaskPtr(new C6SideInspectionTask()));         


        // ── 면 2: Right ───────────────────────────────────
        seq->AddTask(VMF::TaskPtr(new C6SideMoveToFacePositionTask())); 
        seq->AddTask(VMF::TaskPtr(new C6SideInspectionTask()));         

        // ── 면 3: Front ───────────────────────────────────
        // NeedCylOp=1: Gripper→실린더 전환 후 Turn360 진입
        seq->AddTask(VMF::TaskPtr(new C6SideMoveToFacePositionTask())); 
        seq->AddTask(VMF::TaskPtr(new C6SideInspectionTask()));         

        // ── 면 4: Top ─────────────────────────────────────
        seq->AddTask(VMF::TaskPtr(new C6SideMoveToFacePositionTask())); 
        seq->AddTask(VMF::TaskPtr(new C6SideInspectionTask()));         

        // ── 면 5: Rear ────────────────────────────────────
        seq->AddTask(VMF::TaskPtr(new C6SideMoveToFacePositionTask())); 
        seq->AddTask(VMF::TaskPtr(new C6SideInspectionTask()));         

        // ── 면 6: Bottom ──────────────────────────────────
        seq->AddTask(VMF::TaskPtr(new C6SideMoveToFacePositionTask())); 
        seq->AddTask(VMF::TaskPtr(new C6SideInspectionTask()));         

        return VMF::SequencePtr(seq.release());
    }


}
