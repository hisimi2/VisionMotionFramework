#pragma once

#include "CCylinderBase.h"


class CClampReleaseCylinder : public CCylinderBase, public IClampReleaseCylinder
{
public:
	CClampReleaseCylinder(std::string name, int nOnBit = 0, int nOffBit = 0)
		: CCylinderBase(name, nOnBit, nOffBit) {
	}

	int clamp(bool bManual) {
		return actA(bManual);
	}

	int release (bool bManual) {
		return actB(bManual);
	}

	bool isClamp() {
		return isActA();
	}

	bool isRelease() {
		return isActB();
	}
};