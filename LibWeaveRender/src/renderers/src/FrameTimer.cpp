#include "pch.hpp"
#include "FrameTimer.hpp"

using namespace Weave;

FrameTimer::FrameTimer() :
	nativeRefreshCycleNS(0)
{ }

FrameTimer::~FrameTimer() = default;

bool FrameTimer::GetCanUseHighResSleep() const { return limiter.GetCanUseHighResSleep(); }

slong FrameTimer::GetAverageFrameTimeNS() const { return limiter.GetAverageTickTimeNS(); }

slong FrameTimer::GetNativeRefreshCycleNS() const { return nativeRefreshCycleNS; }

void FrameTimer::SetNativeRefreshCycleNS(slong refreshNS) { nativeRefreshCycleNS = refreshNS; }

void FrameTimer::SetTargetFrameTimeNS(slong newTimeNS) { limiter.SetTargetTickTimeNS(newTimeNS); }

slong FrameTimer::GetTargetFrameTimeNS() const { return limiter.GetTargetTickTimeNS(); }

slong FrameTimer::GetFrameCount() const { return limiter.GetTickCount(); }

void FrameTimer::BeginPresent() { limiter.BeginTick(); }

uint FrameTimer::WaitPresent(bool canSync, slong spinLimit)
{
	// VSync enabled
	if (canSync && nativeRefreshCycleNS > 0)
	{
		const slong presentDeltaNS = std::max(limiter.GetTargetTickTimeNS(), nativeRefreshCycleNS.load());
		const slong presentRatio = (presentDeltaNS + nativeRefreshCycleNS / 2) / nativeRefreshCycleNS;
		return std::clamp((uint)presentRatio, 1u, 4u);
	}
	// No VSync
	else
	{
		limiter.WaitTick(spinLimit);
		return 0;
	}
}

void FrameTimer::EndPresent() { limiter.EndTick(); }
