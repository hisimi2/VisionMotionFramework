#pragma once

#include "CCylinderBase.h"


class CNarrowWideCylinder : public CCylinderBase, public INarrowWideCylinder
{
public:
	CNarrowWideCylinder(std::string name, int nOnBit = 0, int nOffBit = 0)
		: CCylinderBase(name, nOnBit, nOffBit) {
	}

	int narrow(bool bManual) {
		return CCylinderBase::actA(bManual);
	}

	int wide(bool bManual) {
		return CCylinderBase::actB(bManual);
	}

	bool isNarrow() {
		return CCylinderBase::isActA();
	}

	bool isWide() {
		return CCylinderBase::isActB();
	}
};

