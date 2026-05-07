#pragma once
#include "CCylinderBase.h"

class CUpDownCylinder : public CCylinderBase, public IUpDownCylinder
{
public:
	CUpDownCylinder(std::string name, int nOnBit = 0, int nOffBit = 0)
		: CCylinderBase(name, nOnBit, nOffBit) {
	}

	int up(bool bManual) {
		return actA(bManual);
	}

	int down(bool bManual) {
		return actB(bManual);
	}

	bool isUp() {
		return isActA();
	}

	bool isDown() {
		return isActB();
	}
};

