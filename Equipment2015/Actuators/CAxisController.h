#pragma once

#include <string>

class IAxisController
{
public:
	virtual ~IAxisController() {}
	virtual int Move(double move_pos, double speed = 0, int accel = 0, int dccel = 0) = 0;
    virtual int MotionDone(int nothrlock = 0) = 0;
    virtual int MoveWhile(double pos, double speed = 0, int accel = 0, int dccel = 0) = 0;
    virtual double GetEncoder() = 0;
    virtual BOOL GetSetvoStat() = 0;
    virtual BOOL GetAmpFaultStat() = 0;
};

class OneAxis : public IAxisController
{
	double      m_encoderPosition;
	std::string m_name;
public:
    OneAxis(std::string name)
	{
		m_name = name;
		m_encoderPosition = 0.0;
	}
    virtual ~OneAxis() {}

    int Move(double move_pos, double speed = 0, int accel = 0, int dccel = 0) override
    {
        m_encoderPosition = move_pos;
        return 1;
    }

    int MotionDone(int nothrlock = 0) override
    {
        return 1;
    }

    int MoveWhile(double pos, double speed = 0, int accel = 0, int dccel = 0) override
    {
        m_encoderPosition = pos;
        return 1;
    }

    double GetEncoder() override
    {
        return m_encoderPosition;
    }

    BOOL GetSetvoStat() override
    {
        return TRUE;
    }

    BOOL GetAmpFaultStat() override
    {
        return FALSE;
    }
};

