#include "StdAfx.h"
#include "VatAdapterLoad1.h"
#include <Windows.h>

namespace VMF_Load1
{
	// ---------------- Constructor ----------------
	VatAdapterLoad1::VatAdapterLoad1(Load1Parts* parts)
		: m_parts(parts)
	{
	}

	VatAdapterLoad1::~VatAdapterLoad1()
	{
	}

	// ---------------- Pitch ----------------
    VMF::PitchType VatAdapterLoad1::GetPitchType()
	{
		return VMF::Variable;
	}

	VMF::ActError VatAdapterLoad1::IsReadyToMove()
	{
		return VMF::ActOk;
	}

	// ---------------- Move ----------------
    VMF::ActError VatAdapterLoad1::MoveZ(double targetZ)
    {
        if (!m_parts) return VMF::ActFail;

        m_parts->LOAD1_Z.Move(targetZ);
        return VMF::ActOk;
    }

    VMF::ActError VatAdapterLoad1::Move(std::vector<double> pos, VMF::PitchStatus action)
    {
        if (!m_parts) return VMF::ActFail;

        // X,Y축이 움직이기 위한 Z축 위치 확인
        //if(m_parts->LOAD1_Z.GetCurrentPosition() > 0) return ActuatorFAIL;

        // X,Y 이동
        m_parts->LOAD1_X.Move(pos[0]);
        m_parts->LOAD1_Y.Move(pos[1]);

        // Pitch 변경
        if (action == VMF::Narrow)
        {
            m_parts->LoadHandYPitch.narrow(true);
        }
        else
        {
            m_parts->LoadHandYPitch.wide(true);
        }

        if (pos.size() > 4)
        {
            // Loader Buffer, Table 이동
            m_parts->LOAD_TABLE1.Move(pos[3]);
            m_parts->LOAD_TABLE2.Move(pos[4]);
        }
        else
        {
            // Loader Buffer, Table 치우기
            m_parts->LOAD_TABLE1.Move(100);
            m_parts->LOAD_TABLE2.Move(100);
        }

        m_parts->LoadBuffer.backward(true);


        return VMF::ActOk;
    }

    VMF::ActError VatAdapterLoad1::isMoveZ(double targetZ)
    {
        double currentposZ = m_parts->LOAD1_Z.GetEncoder();
        double diff = abs(currentposZ - targetZ);

        if (diff > 1)
        {
            return VMF::ActFail;
        }

        return VMF::ActOk;
    }

    VMF::ActError VatAdapterLoad1::isMove(std::vector<double> pos, VMF::PitchStatus action)
    {
        double currentposX = m_parts->LOAD1_X.GetEncoder();
        double currentposY = m_parts->LOAD1_Y.GetEncoder();

        double diffX = abs(currentposX - pos[0]);
        double diffy = abs(currentposY - pos[1]);

        if (diffX > 1 || diffy > 1)
        {
            return VMF::ActFail;
        }

        if (action == VMF::Narrow && !m_parts->LoadHandYPitch.isNarrow())
        {
            return VMF::ActFail;
        }
        else if (action == VMF::Wide && !m_parts->LoadHandYPitch.isWide())
        {
            return VMF::ActFail;
        }


        double currentTablepos1 = m_parts->LOAD_TABLE1.GetEncoder();
        double currentTablepos2 = m_parts->LOAD_TABLE2.GetEncoder();

        if (pos.size() > 4)
        {
            double diffTable1 = abs(currentTablepos1 - pos[3]);
            double diffTable2 = abs(currentTablepos2 - pos[4]);

            if (diffTable1 > 1 || diffTable2 > 1)
            {
                return VMF::ActFail;
            }
        }
        else
        {
            if (currentTablepos1 < 0 || currentTablepos2 < 0)
            {
                return VMF::ActFail;
            }
        }

        if (!m_parts->LoadBuffer.isBackward())
        {
            return  VMF::ActFail;
        }

        return  VMF::ActOk;
    }

	std::vector<double> VatAdapterLoad1::getPosition()
    {
		std::vector<double> vposition;
		vposition.push_back(m_parts->LOAD1_X.GetEncoder());
		vposition.push_back(m_parts->LOAD1_Y.GetEncoder());
		vposition.push_back(m_parts->LOAD1_Z.GetEncoder());

        return vposition;
    }

	std::vector<double> VatAdapterLoad1:: getPulse()
	{
		std::vector<double> vpulse;

		vpulse.push_back(m_parts->LOAD1_X.GetEncoder());
		vpulse.push_back(m_parts->LOAD1_Y.GetEncoder());
		vpulse.push_back(m_parts->LOAD_TABLE1.GetEncoder());
		vpulse.push_back(m_parts->LOAD_TABLE2.GetEncoder());

		return vpulse;
	}


	int VatAdapterLoad1::SetLightState(int camIndex, bool on)
	{
		if (!m_parts) return 0;

        switch (camIndex)
        {
        case 0:
            m_parts->VisionLED_Left.SetStatus(on);
            break;
        case 1:
            m_parts->VisionLED_Right.SetStatus(on);
            break;
        case 2:
            m_parts->VisionLED_Lower.SetStatus(on);
            break;
        }

		return 1;
	}

	int VatAdapterLoad1::GetLightState(int camIndex, bool& outOn)
	{
		if (!m_parts) return 0;

        switch (camIndex)
        {
        case 0:
            outOn = m_parts->VisionLED_Left.GetStatus();
            break;
        case 1:
            outOn = m_parts->VisionLED_Right.GetStatus();
            break;
        case 2:
            outOn = m_parts->VisionLED_Lower.GetStatus();
            break;
        default:
            return 0;
        }

		return 1;
	}
} 
