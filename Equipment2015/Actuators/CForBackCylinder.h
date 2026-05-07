#pragma once

#include "CCylinderBase.h"


class CForBackCylinder : public CCylinderBase, public IForBackCylinder
{
public:
	CForBackCylinder(std::string name, int nOnBit = 0, int nOffBit = 0)
		: CCylinderBase(name, nOnBit, nOffBit) {
	}

	int forward(bool bManual) {
		return CCylinderBase::actA(bManual);
	}

	int backward(bool bManual) {
		return CCylinderBase::actB(bManual);
	}

	bool isForward() {
		return CCylinderBase::isActA();
	}

	bool isBackward() {
		return CCylinderBase::isActB();
	}
};
