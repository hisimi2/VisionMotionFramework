#pragma once
#include "ICylinder.h"
#include <string>
#include <vector>

const unsigned int INVALID_PORT = 0xFFFFFFFFU;

class CCylinderBase : public virtual ICylinder
{
protected:
    std::vector<UINT>   m_ASensor;
    std::vector<UINT>   m_BSensor;
    long long           m_dTimer;
    UINT                m_uDelay;
    std::string         m_strName;
    UINT                m_nOutA;
    UINT                m_nOutB;

public:
    CCylinderBase(const std::string& strName, UINT OutA = INVALID_PORT, UINT OutB = INVALID_PORT)
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

    void setSensorPortA(const std::vector<UINT>& list);
    void setSensorPortB(const std::vector<UINT>& list);

    virtual int  actA(bool bManual);
    virtual int  actB(bool bManual);
    virtual bool isActA();
    virtual bool isActB();

    void setName(const std::string& strData);
    void setDelay(long long dTimer = 3000, UINT uDelay = 1);
};

