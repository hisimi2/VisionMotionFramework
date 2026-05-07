#pragma once

// 전방선언: 실제 정의는 다른 헤더에 존재합니다.
class IPeriodicTask;
class CSubject;

// IOPSwitch가 IPeriodicTask와 CSubject를 상속받아 주기적 실행 및 옵저버 기능을 포함합니다.
class IOPSwitch 
{
public:
    // VS2010(v100) 호환을 위해 enum class 대신 plain enum 사용
    enum EType { KEEP, PUSH, TOGGLE };

    virtual ~IOPSwitch() {}

    // 스위치 고유 기능
    virtual bool getSwitchStatus() = 0;
    virtual void setSwitchStatus(bool bStatus) = 0;
    
    // 설정 관련 메서드 (체이닝 지원)
    virtual IOPSwitch& setGroup(IOPSwitch* pObject) = 0;
    virtual IOPSwitch& setBlink(bool bStatus) = 0;
    virtual IOPSwitch& setOption(EType type, bool isBlink = false, unsigned int pollIntervalMs = 500) = 0;
};