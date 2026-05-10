#pragma once

// VS2010(v100) 호환: constexpr 대신 const 사용
const bool MANUAL_OPERATION = true;

class ICylinder
{
public:
    // 순수 가상 소멸자(정의 필요)
    virtual ~ICylinder() {};

    // 실린더 동작 인터페이스
    virtual int  actA(bool bManual = MANUAL_OPERATION) = 0;
    virtual int  actB(bool bManual = MANUAL_OPERATION) = 0;

    // 상태 확인
    virtual bool isActA() = 0;
    virtual bool isActB() = 0;
};

class IUpDownCylinder 
{
public:
    virtual ~IUpDownCylinder() {}
    virtual int up(bool bManual = MANUAL_OPERATION) = 0;
    virtual int down(bool bManual = MANUAL_OPERATION) = 0;
    virtual bool isUp() = 0;
    virtual bool isDown() = 0;
};

class INarrowWideCylinder
{
public:
	virtual ~INarrowWideCylinder() {}
	virtual int narrow(bool bManual = MANUAL_OPERATION) = 0;
	virtual int wide(bool bManual = MANUAL_OPERATION) = 0;
	virtual bool isNarrow() = 0;
	virtual bool isWide() = 0;
};

class IOpenCloseCylinder 
{
public:
    virtual ~IOpenCloseCylinder() {};
    virtual int open(bool bManual = MANUAL_OPERATION) = 0;
    virtual int close(bool bManual = MANUAL_OPERATION) = 0;
    virtual bool isOpen() = 0;
    virtual bool isClose() = 0;
};

class IForBackCylinder 
{
public:
    virtual ~IForBackCylinder() {};
    virtual int forward(bool bManual = MANUAL_OPERATION) = 0;
    virtual int backward(bool bManual = MANUAL_OPERATION) = 0;
    virtual bool isForward() = 0;
    virtual bool isBackward() = 0;
};

class IClampReleaseCylinder 
{
public:
    virtual ~IClampReleaseCylinder() {};
    virtual int clamp(bool bManual = MANUAL_OPERATION) = 0;
    virtual int release(bool bManual = MANUAL_OPERATION) = 0;
    virtual bool isClamp() = 0;
    virtual bool isRelease() = 0;
};

class IVaccumBlowCylinder 
{
public:
    virtual ~IVaccumBlowCylinder() {};
    virtual int vaccum(bool bManual = MANUAL_OPERATION) = 0;
    virtual int blow(bool bManual = MANUAL_OPERATION) = 0;
    virtual bool isVaccum() = 0;
    virtual bool isBlow() = 0;
};