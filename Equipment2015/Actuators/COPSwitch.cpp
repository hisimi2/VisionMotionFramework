#include "stdafx.h"
#include "COPSwitch.h"

// static 멤버 변수 초기화
IDio* COPSwitch::m_pIo = NULL;

COPSwitch::COPSwitch(const std::string& strName)
    : m_strName(strName)
    , m_status(false)
    , m_toggleFlag(false)
    , m_blinkStatus(false)
    , m_type(IOPSwitch::PUSH)
    , m_isBlink(false)
    , m_pGroup(NULL)
    , m_pollIntervalMs(10)
    , m_BlinkTimer(NULL)
{
}

COPSwitch::~COPSwitch()
{
    if (m_BlinkTimer)
    {
        delete m_BlinkTimer;
        m_BlinkTimer = NULL;
    }
}

void COPSwitch::setIo(IDio* pIo)
{
    m_pIo = pIo;
}

IOPSwitch& COPSwitch::setGroup(IOPSwitch* pObject)
{
    m_pGroup = pObject;
    return *this;
}

IOPSwitch& COPSwitch::setBlink(bool bStatus)
{
    m_isBlink = bStatus;
    return *this;
}

IOPSwitch& COPSwitch::setOption(IOPSwitch::EType type, bool isBlink, unsigned int pollIntervalMs)
{
    m_type = type;
    m_isBlink = isBlink;
    m_pollIntervalMs = pollIntervalMs;

    if (m_BlinkTimer)
    {
        delete m_BlinkTimer;
        m_BlinkTimer = NULL;
    }
    m_BlinkTimer = new CTimer(static_cast<long long>(m_pollIntervalMs));
    return *this;
}

void COPSwitch::setInput(const std::vector<int>& inputs)
{
    m_inputs = inputs;
}

void COPSwitch::setOutput(const std::vector<int>& outputs)
{
    m_outputs = outputs;
}

bool COPSwitch::getStatus()
{
    std::lock_guard<std::mutex> lock(m_logicMutex);
    return m_status;
}

void COPSwitch::setStatus(bool bStatus)
{
    std::lock_guard<std::mutex> lock(m_logicMutex);
    if (m_status != bStatus)
    {
        m_status = bStatus;

        if (m_pGroup)
        {
            if (bStatus)
            {
                m_pGroup->setStatus(false);
            }
        }
    }
}

bool COPSwitch::sequence()
{
    std::lock_guard<std::mutex> lock(m_logicMutex);
    bool in_sensor = checkInSensor();

    switch (m_type)
    {
    case IOPSwitch::KEEP:
        setStatus(in_sensor);
        break;
    case IOPSwitch::PUSH:
        setStatus(in_sensor);
        break;
    case IOPSwitch::TOGGLE:
        if (in_sensor && !m_toggleFlag)
        {
            m_toggleFlag = true;
            setStatus(!getStatus());
        }
        else if (!in_sensor)
        {
            m_toggleFlag = false;
        }
        break;
    default:
        break;
    }

    if (m_isBlink)
    {
        if (getStatus())
        {
            if (m_BlinkTimer && m_BlinkTimer->isOver())
            {
                m_blinkStatus = !m_blinkStatus;
                setLED(m_blinkStatus);
            }
        }
        else
        {
            setLED(false);
        }
    }
    else
    {
        setLED(getStatus());
    }

    if (m_pGroup)
    {
        if (getStatus())
        {
            m_pGroup->setStatus(false);
        }
    }

    return true;
}

void COPSwitch::setLED(bool bStatus)
{
    if (m_pIo)
    {
        for (size_t i = 0; i < m_outputs.size(); ++i)
        {
            m_pIo->out(m_outputs[i], bStatus);
        }
    }
}

bool COPSwitch::checkInSensor()
{
    if (m_pIo)
    {
        for (size_t i = 0; i < m_inputs.size(); ++i)
        {
            if (m_pIo->in(m_inputs[i])) return true;
        }
    }
    return false;
}
