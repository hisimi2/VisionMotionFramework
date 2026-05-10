#pragma once
#include "ICylinder.h"
#include <string>
#include <vector>

// using 선언으로 코드 간결화
using std::string;
using std::vector;

// VS2010(v100) 호환: constexpr -> const
const unsigned int INVALID_PORT = 0xFFFFFFFFU;

// ICylinder를 가상 상속하여 다이아몬드 문제 해결
class CCylinderBase : public virtual ICylinder
{
protected:
    vector<UINT>    m_ASensor;
    vector<UINT>    m_BSensor;
    long long       m_dTimer;
    UINT            m_uDelay;
    string          m_strName;
    UINT            m_nOutA;
    UINT            m_nOutB;

public:
    // 모든 멤버는 생성자에서 초기화 (in-class 초기화 제거)
    CCylinderBase(const string& strName, UINT OutA = INVALID_PORT, UINT OutB = INVALID_PORT)
        : m_ASensor()
        , m_BSensor()
        , m_dTimer(0)
        , m_uDelay(3000)
        , m_strName(strName)
        , m_nOutA(OutA)
        , m_nOutB(OutB)
    {
    }

    virtual ~CCylinderBase() {}

    // VS2010 호환: std::initializer_list 제거 -> const vector<UINT>& 사용
    void setSensorPortA(const vector<UINT>& list);
    void setSensorPortB(const vector<UINT>& list);

    // override 키워드 제거 및 기본값은 인터페이스(ICylinder)에만 유지
    virtual int  actA(bool bManual);
    virtual int  actB(bool bManual);
    virtual bool isActA();
    virtual bool isActB();

    void setName(const string& strData);
    void setDelay(long long dTimer = 3000, UINT uDelay = 1);
};

