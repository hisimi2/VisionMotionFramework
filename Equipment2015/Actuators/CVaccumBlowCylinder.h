#pragma once

#pragma once
#include "CCylinderBase.h"

class CVaccumBlowCylinder : public CCylinderBase, public IVaccumBlowCylinder
{
public:
	CVaccumBlowCylinder(std::string name, int nOnBit = 0, int nOffBit = 0)
		: CCylinderBase(name, nOnBit, nOffBit) {
	}

	int vaccum(bool bManual) {
		return CCylinderBase::actA(bManual);
	}

	int blow(bool bManual) {
		return CCylinderBase::actB(bManual);
	}

	bool isVaccum() {
		return CCylinderBase::isActA();
	}

	bool isBlow() {
		return CCylinderBase::isActB();
	}
};