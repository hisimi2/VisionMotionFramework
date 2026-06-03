#include "stdafx.h"
#include "CCylinderBase.h"
#include "CTimer.h"

// 센서 포트 설정: 헤더와 일치하도록 const vector<UINT>& 사용
void CCylinderBase::setSensorPortA(const std::vector<UINT>& list)
{
	m_ASensor = list;
}

void CCylinderBase::setSensorPortB(const std::vector<UINT>& list)
{
	m_BSensor = list;
}

bool CCylinderBase::isActA()
{
	bool bPortA = true;
	bool bPortB = false;

	if (m_ASensor.empty() && m_BSensor.empty())
	{
		if (INVALID_PORT != m_nOutA)
		{
			// bPortA = m_pDIO->out(m_nOutA);
		}

		if (INVALID_PORT != m_nOutB)
		{
			// bPortB = m_pDIO->out(m_nOutB);
		}
	}

	for (size_t nIndex = 0; nIndex < m_ASensor.size(); ++nIndex)
	{
		if (1) // !m_pDIO->in(m_ASensor[nIndex])
		{
			bPortA = false;
			break;
		}
	}
	for (size_t nIndex = 0; nIndex < m_BSensor.size(); ++nIndex)
	{
		if (1) // m_pDIO->in(m_BSensor[nIndex])
		{
			bPortB = true;
			break;
		}
	}

	return (bPortA && !bPortB) ? true : false;
}

bool CCylinderBase::isActB()
{
	bool bPortA = false;
	bool bPortB = true;

	if (m_ASensor.empty() && m_BSensor.empty())
	{
		if (INVALID_PORT != m_nOutA)
		{
			// bPortA = m_pDIO->out(m_nOutA);
		}

		if (INVALID_PORT != m_nOutB)
		{
			// bPortB = m_pDIO->out(m_nOutB);
		}
	}
	else
	{
		for (size_t nIndex = 0; nIndex < m_ASensor.size(); ++nIndex)
		{
			if (1) // m_pDIO->in(m_ASensor[nIndex])
			{
				bPortA = true;
				break;
			}
		}
		for (size_t nIndex = 0; nIndex < m_BSensor.size(); ++nIndex)
		{
			if (1) // !m_pDIO->in(m_BSensor[nIndex])
			{
				bPortB = false;
				break;
			}
		}
	}

	return (!bPortA && bPortB) ? true : false;
}

void CCylinderBase::setName(const std::string& strData)
{
	m_strName = strData;
}

int CCylinderBase::actA(bool bManual)
{
	CTimer checkTimeOut(static_cast<long long>(m_dTimer));
	checkTimeOut.startTimer();

	for (;;)
	{
		if (isActA())
		{
			Sleep(m_uDelay);
			return 1;
		}
		else
		{
			if (checkTimeOut.isOver())
			{
				return 0;
			}
			Sleep(10);  // CPU 점유율 방지
		}
	}
}

int CCylinderBase::actB(bool bManual)
{
	CTimer checkTimeOut(static_cast<long long>(m_dTimer));
	checkTimeOut.startTimer();

	for (;;)
	{
		if (isActB())
		{
			Sleep(m_uDelay);
			return 1;
		}
		else
		{
			if (checkTimeOut.isOver())
			{
				return 0;
			}
			Sleep(10);  // CPU 점유율 방지
		}
	}
}

void CCylinderBase::setDelay(long long dTimer, UINT uDelay)
{
	m_dTimer = dTimer;
	m_uDelay = uDelay;
}