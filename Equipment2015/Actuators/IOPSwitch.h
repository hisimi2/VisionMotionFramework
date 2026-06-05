#pragma once

class IPeriodicTask;
class CSubject;

class IOPSwitch 
{
public:
    enum EType { KEEP, PUSH, TOGGLE };

    virtual ~IOPSwitch() {}

    // 스위치 고유 기능
    virtual bool getStatus() = 0;
    virtual void setStatus(bool bStatus) = 0;
    
    // 설정 관련 메서드 (체이닝 지원)
    virtual IOPSwitch& setGroup(IOPSwitch* pObject) = 0;
    virtual IOPSwitch& setBlink(bool bStatus) = 0;
    virtual IOPSwitch& setOption(EType type, bool isBlink = false, unsigned int pollIntervalMs = 500) = 0;
};
