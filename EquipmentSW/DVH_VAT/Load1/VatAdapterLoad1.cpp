#include "StdAfx.h"
#include "VatAdapterLoad1.h"
#include <Windows.h>

namespace VAT_LOAD1
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
    DVH_VAT::PitchType VatAdapterLoad1::GetPitchType()
	{
		return DVH_VAT::Variable;
	}

	DVH_VAT::ActError VatAdapterLoad1::IsReadyToMove()
	{
		return DVH_VAT::ActOk;
	}

	// ---------------- Move ----------------
    DVH_VAT::ActError VatAdapterLoad1::MoveZ(double targetZ)
    {
        if (!m_parts) return DVH_VAT::ActFail;

        m_parts->LOAD1_Z.Move(targetZ);
        return DVH_VAT::ActOk;
    }

    DVH_VAT::ActError VatAdapterLoad1::Move(std::vector<double> pos, DVH_VAT::PitchStatus action)
    {
        if (!m_parts) return DVH_VAT::ActFail;

        // X,Y축이 움직이기 위한 Z축 위치 확인
        //if(m_parts->LOAD1_Z.GetCurrentPosition() > 0) return ActuatorFAIL;

        // X,Y 이동
        m_parts->LOAD1_X.Move(pos[0]);
        m_parts->LOAD1_Y.Move(pos[1]);

        // Pitch 변경
        if (action == DVH_VAT::Narrow)
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


        return DVH_VAT::ActOk;
    }

    DVH_VAT::ActError VatAdapterLoad1::isMoveZ(double targetZ)
    {
        double currentposZ = m_parts->LOAD1_Z.GetEncoder();
        double diff = abs(currentposZ - targetZ);

        if (diff > 1)
        {
            return DVH_VAT::ActFail;
        }

        return DVH_VAT::ActOk;
    }

    DVH_VAT::ActError VatAdapterLoad1::isMove(std::vector<double> pos, DVH_VAT::PitchStatus action)
    {
        double currentposX = m_parts->LOAD1_X.GetEncoder();
        double currentposY = m_parts->LOAD1_Y.GetEncoder();

        double diffX = abs(currentposX - pos[0]);
        double diffy = abs(currentposY - pos[1]);

        if (diffX > 1 || diffy > 1)
        {
            return DVH_VAT::ActFail;
        }

        if (action == DVH_VAT::Narrow && !m_parts->LoadHandYPitch.isNarrow())
        {
            return DVH_VAT::ActFail;
        }
        else if (action == DVH_VAT::Wide && !m_parts->LoadHandYPitch.isWide())
        {
            return DVH_VAT::ActFail;
        }


        double currentTablepos1 = m_parts->LOAD_TABLE1.GetEncoder();
        double currentTablepos2 = m_parts->LOAD_TABLE2.GetEncoder();

        if (pos.size() > 4)
        {
            double diffTable1 = abs(currentTablepos1 - pos[3]);
            double diffTable2 = abs(currentTablepos2 - pos[4]);

            if (diffTable1 > 1 || diffTable2 > 1)
            {
                return DVH_VAT::ActFail;
            }
        }
        else
        {
            if (currentTablepos1 < 0 || currentTablepos2 < 0)
            {
                return DVH_VAT::ActFail;
            }
        }

        if (!m_parts->LoadBuffer.isBackward())
        {
            return  DVH_VAT::ActFail;
        }

        return  DVH_VAT::ActOk;
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
