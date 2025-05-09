#pragma once
#include "WeaveUtils/TickLimiter.hpp"

namespace Weave
{
	/// <summary>
	/// Frame timing class for high-precision thread sleeping for frame rate limiting
	/// </summary>
	class FrameTimer
	{
	public:
		FrameTimer();

		~FrameTimer();

		/// <summary>
		/// Returns true if thread sleep interrupts can be configured to a 1ms tick for WaitPresent().
		/// Thread safe.
		/// </summary>
		bool GetCanUseHighResSleep() const;

		/// <summary>
		/// Returns the average frame time in nanoseconds.  Thread safe.
		/// </summary>
		slong GetAverageFrameTimeNS() const;

		/// <summary>
		/// Returns the last set native refresh cycle time of the monitor in nanoseconds.  Thread safe.
		/// </summary>
		slong GetNativeRefreshCycleNS() const;

		/// <summary>
		/// Sets the native refresh cycle of the monitor in nanoseconds for use with VSync.  Thread safe.
		/// </summary>
		void SetNativeRefreshCycleNS(slong refreshNS);
		
		/// <summary>
		/// Sets the target frame time in nanoseconds. Thread safe.
		/// </summary>
		void SetTargetFrameTimeNS(slong newTimeNS);

		/// <summary>
		/// Returns the target frame time in nanoseconds. Thread safe.
		/// </summary>
		slong GetTargetFrameTimeNS() const;

		/// <summary>
		/// Returns the number of rendered frames tracked by the timer. Thread safe.
		/// </summary>
		slong GetFrameCount() const;

		/// <summary>
		/// Marks the beginning of a new frame and updates timers. Not thread safe.
		/// </summary>
		void BeginPresent();

		/// <summary>
		/// Used to wait immediately before present to limit maximum frame rate based on set
		/// target frame time, or returns a non-zero sync interval if it aligns with the target
		/// frame rate. Not thread safe.
		/// </summary>
		uint WaitPresent(bool canSync = false, slong spinLimit = slong(2E5));

		/// <summary>
		/// Marks the end of the last frame and updates timers. Not thread safe.
		/// </summary>
		void EndPresent();

	private:
		TickLimiter limiter;

		std::atomic<slong> nativeRefreshCycleNS;

	};
}