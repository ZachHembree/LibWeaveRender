#pragma once
#include <atomic>
#include "WeaveUtils/GlobalUtils.hpp"
#include "WeaveUtils/Stopwatch.hpp"

namespace Weave
{
	/// <summary>
	/// Update loop timing class for high-precision thread sleeping for tick rate limiting.
	/// </summary>
	class TickLimiter
	{
	public:
		TickLimiter();

		~TickLimiter();

		/// <summary>
		/// Returns true if thread sleep interrupts can be configured to a 1ms tick for WaitTick()
		/// Thread safe.
		/// </summary>
		bool GetCanUseHighResSleep() const;

		/// <summary>
		/// Returns the average tick time in nanoseconds. Thread safe.
		/// </summary>
		slong GetAverageTickTimeNS() const;

		/// <summary>
		/// Sets the target tick time in nanoseconds. Thread safe.
		/// </summary>
		void SetTargetTickTimeNS(slong newTimeNS);

		/// <summary>
		/// Returns the target tick time in nanoseconds. Thread safe.
		/// </summary>
		slong GetTargetTickTimeNS() const;

		/// <summary>
		/// Returns the number of ticks tracked by the timer. Thread safe.
		/// </summary>
		slong GetTickCount() const;

		/// <summary>
		/// Marks the beginning of an update tick. Not thread safe.
		/// </summary>
		void BeginTick();

		/// <summary>
		/// Used to wait immediately before update. Not thread safe. Setting a non-zero spin limit will 
		/// allow waits to spin in a busy wait to fine-tune update times. 0.2ms or 2E5ns is roughly the 
		/// smallest useful value. Otherwise, leave it off.
		/// </summary>
		void WaitTick(slong spinLimit = 0ll);

		/// <summary>
		/// Marks the end of the last tick. Not thread safe.
		/// </summary>
		void EndTick();

	private:
		std::atomic<slong> targetTickTimeNS;

		bool canHighResSleep;
		uint sysInterruptTickMS;

		slong targetTickNS;
		slong lastTickNS;

		slong avgTickErrorNS;
		slong avgSpinErrorNS;
		std::atomic<slong> avgTickDeltaNS;

		Stopwatch tickTimer;
		std::atomic<slong> tickCount;
	};
}