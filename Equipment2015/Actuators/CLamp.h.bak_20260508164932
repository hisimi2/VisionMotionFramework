#pragma once

#include <string>

class CLamp 
{
public:
    CLamp(const std::string& name = "") : m_name(name), m_on(false){}
    virtual ~CLamp() {}

    void SetStatus(bool on)
    {
         m_on = on;
    }

    bool GetStatus() const
    {
        return m_on;
    }

private:
    std::string m_name;
    bool m_on;
    
};

