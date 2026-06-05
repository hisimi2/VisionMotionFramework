#pragma once

#include <string>
#include "IOPSwitch.h"
#include "IDio.h"
#include "CTimer.h"
#include <vector>
#include <mutex>

class COPSwitch : public IOPSwitch 
{
public:
    COPSwitch(const std::string& strName);
    virtual ~COPSwitch();

    // IOPSwitch (및 IPeriodicTask) 인터페이스 구현
    bool getStatus();
    void setStatus(bool bStatus);
    
    IOPSwitch& setGroup(IOPSwitch* pObject);
    IOPSwitch& setBlink(bool bStatus);
    IOPSwitch& setOption(EType type, bool isBlink = false, unsigned int pollIntervalMs = 500);

    // IPeriodicTask 구현
    bool sequence();

    // COPSwitch 고유 메서드
    static void setIo(IDio* pIo);
    void setInput(const std::vector<int>& inputs);
    void setOutput(const std::vector<int>& outputs);

private:
    std::string m_strName;
    void setLED(bool bStatus);
    bool checkInSensor();
    static IDio* m_pIo;

    std::vector<int> m_inputs;
    std::vector<int> m_outputs;

    bool m_status;
    bool m_toggleFlag;
    bool m_blinkStatus;
    EType m_type;
    bool m_isBlink;
    IOPSwitch* m_pGroup;
    unsigned int m_pollIntervalMs;

    CTimer* m_BlinkTimer;
    std::mutex m_logicMutex;
};

