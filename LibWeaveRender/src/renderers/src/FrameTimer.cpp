#include "pch.hpp"
#include "WeaveUtils/Win32.hpp"
#include "FrameTimer.hpp"
#include <thread>
#include <timeapi.h>
#include <intrin.h>
#pragma comment(lib, "winmm.lib")

using namespace Weave;

FrameTimer::FrameTimer() :
	targetPresentNS(0),
	nativeRefreshCycleNS(0),
	targetFrameTimeNS(0),
	canHighResSleep(false),
	avgPresentErrorNS(0),
	avgSpinErrorNS(0),
	avgFrameDeltaNS(0),
	lastPresentNS(0),
	frameCount(0)
{
	UINT desiredResolutionMS = 1;
	TIMECAPS tc;

	// Query interrupt timer resolution
	if (timeGetDevCaps(&tc, sizeof(TIMECAPS)) == TIMERR_NOERROR)
	{
		sysInterruptTickMS = std::min(std::max(tc.wPeriodMin, desiredResolutionMS), tc.wPeriodMax);
		timeBeginPeriod(sysInterruptTickMS);
		canHighResSleep = true;
	}
	else
		sysInterruptTickMS = 10;
	
	frameTimer.Start();
}

FrameTimer::~FrameTimer()
{
	if (canHighResSleep)
		timeEndPeriod(sysInterruptTickMS);
}

bool FrameTimer::GetCanUseHighResSleep() const { return canHighResSleep; }

slong FrameTimer::GetAverageFrameTimeNS() const { return avgFrameDeltaNS; }

slong FrameTimer::GetNativeRefreshCycleNS() const { return nativeRefreshCycleNS; }

void FrameTimer::SetNativeRefreshCycleNS(slong refreshNS) { nativeRefreshCycleNS = refreshNS; }

void FrameTimer::SetTargetFrameTimeNS(slong newTimeNS) { targetFrameTimeNS = newTimeNS; }

slong FrameTimer::GetTargetFrameTimeNS() const { return targetFrameTimeNS; }

slong FrameTimer::GetFrameCount() const { return frameCount; }

void FrameTimer::BeginPresent()
{
	targetPresentNS = lastPresentNS + targetFrameTimeNS - avgPresentErrorNS;
}

uint FrameTimer::WaitPresent(bool canSync, bool canSpin, bool canSleep)
{
	// VSync enabled
	if (canSync && nativeRefreshCycleNS > 0)
	{
		const slong presentDeltaNS = std::max(targetFrameTimeNS, nativeRefreshCycleNS);
		const slong presentRatio = (presentDeltaNS + nativeRefreshCycleNS / 2) / nativeRefreshCycleNS;

		return std::clamp((uint)presentRatio, 1u, 4u);
	}
	// No VSync
	else
	{
		const slong sysTickMaxNS = slong(2E6) * sysInterruptTickMS;

		// Target time invalid or too fast to wait efficiently
		if (targetFrameTimeNS <= sysTickMaxNS || targetFrameTimeNS <= 0)
			return 0;

		const slong sleepStartNS = frameTimer.GetElapsedNS();
		// Bias toward sleep based on avg spin time
		const slong sleepEndNS = targetPresentNS + avgSpinErrorNS;
		slong waitNS = std::max(sleepEndNS - sleepStartNS, 0ll);

		while (canSleep && waitNS > sysTickMaxNS)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(sysInterruptTickMS));
			waitNS = std::max(sleepEndNS - frameTimer.GetElapsedNS(), 0ll);
		}

		if (canSpin)
		{
			const slong spinStartNS = frameTimer.GetElapsedNS();
			waitNS = std::max(targetPresentNS - frameTimer.GetElapsedNS(), 0ll);

			while (waitNS > 0)
			{
				_mm_pause();
				waitNS = std::max(targetPresentNS - frameTimer.GetElapsedNS(), 0ll);
			}

			const slong spinErrorNS = frameTimer.GetElapsedNS() - spinStartNS;
			avgSpinErrorNS = static_cast<slong>(0.9 * avgSpinErrorNS + 0.1 * spinErrorNS);
		}

		return 0;
	}
}

void FrameTimer::EndPresent()
{
	frameCount++;

	const slong presentTimeNS = frameTimer.GetElapsedNS();
	slong frameDeltaNS = presentTimeNS - lastPresentNS;
	// Positive if present took longer than expected, negative if too fast
	slong presentErrorNS = presentTimeNS - targetPresentNS;

	if (avgFrameDeltaNS > 0)
		frameDeltaNS = std::clamp(frameDeltaNS, (avgFrameDeltaNS / 3), (3 * avgFrameDeltaNS));

	if (avgPresentErrorNS > 0)
		presentErrorNS = std::clamp(presentErrorNS, (avgPresentErrorNS / 3), (3 * avgPresentErrorNS));

	lastPresentNS = presentTimeNS;
	avgFrameDeltaNS = static_cast<slong>(0.9 * avgFrameDeltaNS + 0.1 * frameDeltaNS);
	avgPresentErrorNS = static_cast<slong>(0.9 * avgPresentErrorNS + 0.1 * presentErrorNS);
}
