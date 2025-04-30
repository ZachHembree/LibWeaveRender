#pragma once
#include "WeaveUtils/Utils.hpp"
#include "WeaveUtils/Stopwatch.hpp"

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
		/// Returns true if thread sleep interrupts can be configured to a 1ms tick for WaitPresent()
		/// </summary>
		bool GetCanUseHighResSleep() const;

		/// <summary>
		/// Returns the average frame time in nanoseconds.
		/// </summary>
		slong GetAverageFrameTimeNS() const;

		/// <summary>
		/// Returns the last set native refresh cycle time of the monitor in nanoseconds.
		/// </summary>
		slong GetNativeRefreshCycleNS() const;

		/// <summary>
		/// Sets the native refresh cycle of the monitor in nanoseconds for use with VSync.
		/// </summary>
		void SetNativeRefreshCycleNS(slong refreshNS);
		
		/// <summary>
		/// Sets the target frame time in nanoseconds
		/// </summary>
		void SetTargetFrameTimeNS(slong newTimeNS);

		/// <summary>
		/// Returns the target frame time in nanoseconds
		/// </summary>
		slong GetTargetFrameTimeNS() const;

		/// <summary>
		/// Returns the number of rendered frames tracked by the timer
		/// </summary>
		slong GetFrameCount() const;

		/// <summary>
		/// Marks the beginning of a new frame and updates timers
		/// </summary>
		void BeginPresent();

		/// <summary>
		/// Used to wait immediately before present to limit maximum frame rate based on set
		/// target frame time, or returns a non-zero sync interval if it aligns with the target
		/// frame rate.
		/// </summary>
		uint WaitPresent(bool canSync = false, bool canSpin = true, bool canSleep = true);

		/// <summary>
		/// Marks the end of the last frame and updates timers
		/// </summary>
		void EndPresent();

	private:
		slong nativeRefreshCycleNS;
		slong targetFrameTimeNS;

		bool canHighResSleep;
		uint sysInterruptTickMS;

		slong targetPresentNS;
		slong lastPresentNS;

		slong avgPresentErrorNS;
		slong avgSpinErrorNS;
		slong avgFrameDeltaNS;

		Stopwatch frameTimer;
		ulong frameCount;
	};
}