#pragma once
#include "CCylinderBase.h"


class COpenCloseCylinder : public CCylinderBase, public IOpenCloseCylinder
{
public: 
	COpenCloseCylinder(std::string name, int nOnBit = 0, int nOffBit = 0)
		: CCylinderBase(name, nOnBit, nOffBit){}

	int open(bool bManual) {
		return CCylinderBase::actA(bManual);
	}

	int close(bool bManual)	{
		return CCylinderBase::actB(bManual);
	}

	bool isOpen() {
		return CCylinderBase::isActA();
	}

	bool isClose() {
		return CCylinderBase::isActB();
	}
};

