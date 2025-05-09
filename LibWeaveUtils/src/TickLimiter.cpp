#include "pch.hpp"
#include "WeaveUtils/Win32.hpp"
#include "WeaveUtils/TickLimiter.hpp"
#include <thread>
#include <timeapi.h>
#include <intrin.h>
#pragma comment(lib, "winmm.lib")

using namespace Weave;

static slong GetEMA(slong value, slong avgValue)
{
	return static_cast<slong>(0.9 * avgValue + 0.1 * value);
}

static slong GetClampedEMA(slong value, slong avgValue)
{
	if (avgValue > 0)
		value = std::clamp(value, avgValue / 3, 3 * avgValue);

	return static_cast<slong>(0.9 * avgValue + 0.1 * value);
}

static slong GetSignedClampedEMA(slong value, slong avgValue)
{
	const slong absErrRange = std::min(3ll * std::abs(avgValue), slong(1E6));

	if (absErrRange > 0)
		value = std::clamp(value, -3 * absErrRange, 3 * absErrRange);

	return static_cast<slong>(0.9 * avgValue + 0.1 * value);
}

TickLimiter::TickLimiter() :
	targetTickNS(0),
	targetTickTimeNS(0),
	canHighResSleep(false),
	avgTickErrorNS(slong(1E6)),
	avgSpinErrorNS(slong(1E6)),
	avgTickDeltaNS(slong(1E6)),
	lastTickNS(0),
	tickCount(0)
{
	UINT desiredResolutionMS = 1;
	TIMECAPS tc;

	// Query interrupt timer resolution
	if (timeGetDevCaps(&tc, sizeof(TIMECAPS)) == TIMERR_NOERROR)
	{
		sysInterruptTickMS = std::min(std::max(tc.wPeriodMin, desiredResolutionMS), tc.wPeriodMax);

		if (timeBeginPeriod(sysInterruptTickMS) == TIMERR_NOERROR)
			canHighResSleep = true;
	}
	else
		sysInterruptTickMS = 10;

	tickTimer.Start();
}

TickLimiter::~TickLimiter()
{
	if (canHighResSleep)
		timeEndPeriod(sysInterruptTickMS);
}

bool TickLimiter::GetCanUseHighResSleep() const { return canHighResSleep; }

slong TickLimiter::GetAverageTickTimeNS() const { return avgTickDeltaNS; }

void TickLimiter::SetTargetTickTimeNS(slong newTimeNS) { targetTickTimeNS = newTimeNS; }

slong TickLimiter::GetTargetTickTimeNS() const { return targetTickTimeNS; }

slong TickLimiter::GetTickCount() const { return tickCount; }

void TickLimiter::BeginTick()
{
	targetTickNS = lastTickNS + targetTickTimeNS - avgTickErrorNS;
}

void TickLimiter::WaitTick(slong spinLimit)
{
	// Convert to MS and double
	const slong sysTickMaxNS = slong(2E6) * sysInterruptTickMS;

	// Target time invalid or too fast to wait efficiently
	if (canHighResSleep && (targetTickTimeNS <= sysTickMaxNS || targetTickTimeNS <= 0))
		return;

	const slong sleepStartNS = tickTimer.GetElapsedNS();
	const slong sleepMinNS = (spinLimit > 0) ? sleepStartNS + sysTickMaxNS : 0;
	// Bias toward sleep based on avg spin time
	const slong sleepEndNS = std::max(targetTickNS, sleepMinNS) + avgSpinErrorNS;
	slong waitNS = std::max(sleepEndNS - sleepStartNS, 0ll);

	// Precision sleep, not perfect, but usually fine
	while (waitNS > sysTickMaxNS)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(sysInterruptTickMS));
		waitNS = std::max(sleepEndNS - tickTimer.GetElapsedNS(), 0ll);
	}

	// Spin for precise timing, if allowed
	const slong spinStartNS = tickTimer.GetElapsedNS();
	waitNS = std::clamp(targetTickNS - tickTimer.GetElapsedNS(), 0ll, spinLimit);

	while (waitNS > slong(1E5))
	{
		_mm_pause();
		waitNS = std::max(targetTickNS - tickTimer.GetElapsedNS(), 0ll);
	}

	avgSpinErrorNS = GetSignedClampedEMA(tickTimer.GetElapsedNS() - spinStartNS, avgSpinErrorNS);
}

void TickLimiter::EndTick()
{
	const slong tickTimeNS = tickTimer.GetElapsedNS();

	avgTickDeltaNS = GetClampedEMA(tickTimeNS - lastTickNS, avgTickDeltaNS);
	// Positive if tick took longer than expected, negative if too fast
	avgTickErrorNS = GetSignedClampedEMA(tickTimeNS - targetTickNS, avgTickErrorNS);
	lastTickNS = tickTimeNS;

	tickCount++;
}
