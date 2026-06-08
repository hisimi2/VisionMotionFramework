#pragma once
#include "CCylinderBase.h"

// ============================================================================
// 실린더 타입 확장 매크로
//
// 사용법: DEFINE_CYLINDER(ClassName, ActionA, ActionB, IsActionA, IsActionB)
//
// ClassName  : 생성할 클래스 이름 (예: CUpDownCylinder)
// ActionA    : actA()에 대응하는 동작 이름 (예: up)
// ActionB    : actB()에 대응하는 동작 이름 (예: down)
// IsActionA  : isActA()에 대응하는 상태 확인 이름 (예: isUp)
// IsActionB  : isActB()에 대응하는 상태 확인 이름 (예: isDown)
// ============================================================================
#define DEFINE_CYLINDER(ClassName, ActionA, ActionB, IsActionA, IsActionB) \
class ClassName : public CCylinderBase \
{ \
public: \
    ClassName(const std::string& name, UINT nOnBit = INVALID_PORT, UINT nOffBit = INVALID_PORT) \
        : CCylinderBase(name, nOnBit, nOffBit) {} \
    int ActionA(bool bManual) { return CCylinderBase::actA(bManual); } \
    int ActionB(bool bManual) { return CCylinderBase::actB(bManual); } \
    bool IsActionA() { return CCylinderBase::isActA(); } \
    bool IsActionB() { return CCylinderBase::isActB(); } \
};

// ============================================================================
// 기존 6개 실린더 타입
// ============================================================================
DEFINE_CYLINDER(CUpDownCylinder,        up,      down,      isUp,      isDown)
DEFINE_CYLINDER(CNarrowWideCylinder,    narrow,  wide,      isNarrow,  isWide)
DEFINE_CYLINDER(COpenCloseCylinder,     open,    close,     isOpen,    isClose)
DEFINE_CYLINDER(CForBackCylinder,       forward, backward,  isForward, isBackward)
DEFINE_CYLINDER(CClampReleaseCylinder,  clamp,   release,   isClamp,   isRelease)
DEFINE_CYLINDER(CVaccumBlowCylinder,    vaccum,  blow,      isVaccum,  isBlow)

// ============================================================================
// 신규 실린더 타입 추가 예시 (필요 시 주석 해제하여 사용)
// ============================================================================
// DEFINE_CYLINDER(CPushPullCylinder, push, pull, isPush, isPull)
// DEFINE_CYLINDER(CLiftLowerCylinder, lift, lower, isLift, isLower)
// DEFINE_CYLINDER(CRotateTurnCylinder, rotate, turn, isRotate, isTurn)
