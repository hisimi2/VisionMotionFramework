#pragma once

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
