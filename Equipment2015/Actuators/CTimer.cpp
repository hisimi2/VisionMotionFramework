#include "stdafx.h"
#include "CTimer.h"
#include <ctime>

CTimer::CTimer(long long checkTimeMs)
	: m_checkTimeMs(static_cast<unsigned long long>(checkTimeMs))
{
	startTimer();
}

bool CTimer::isOver()
{
	std::clock_t now = std::clock();

	unsigned long long elapsedMs = 0;
	if (now >= m_start)
    {
		elapsedMs = static_cast<unsigned long long>(
			(static_cast<unsigned long long>(now - m_start) * 1000ULL) / CLOCKS_PER_SEC);
	} else
    {
		// 드문 경우의 래핑(혹은 시스템 시계 문제)에 대비: 타이머 재시작으로 처리
		startTimer();
		return false;
	}

	return elapsedMs >= m_checkTimeMs;
}

void CTimer::startTimer()
{
	m_start = std::clock();
}
