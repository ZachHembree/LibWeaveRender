#pragma once
#include <chrono>
#include "WeaveUtils/Int.hpp"

namespace Weave
{
	/// <summary>
	/// Converts time in nanoseconds to time in milliseconds
	/// </summary>
	inline static constexpr double GetTimeNStoMS(slong timeNS) { return (double)timeNS * 1E-6; }

	/// <summary>
	/// Converts time in nanoseconds to seconds
	/// </summary>
	inline static constexpr double GetTimeNStoS(slong timeNS) { return (double)timeNS * 1E-9; }

	/// <summary>
	/// Converts a time period in nanoseconds to oscillations per second or Hz
	/// </summary>
	inline static constexpr double GetTimeNStoHZ(slong timeNS) { return (timeNS > 0) ? 1.0 / GetTimeNStoS(timeNS) : 0; }

	/// <summary>
	/// Converts time in milliseconds to nanoseconds
	/// </summary>
	inline static constexpr slong GetTimeMStoNS(double timeMS) { return (slong)(timeMS * 1E6); }

	/// <summary>
	/// Converts time in seconds to nanoseconds
	/// </summary>
	inline static constexpr slong GetTimeStoNS(double timeS) { return (slong)(timeS * 1E9); }

	/// <summary>
	/// Converts a period frequency in Hz to nanoseconds
	/// </summary>
	inline static constexpr slong GetTimeHZtoNS(double freqHz) { return (freqHz > 0) ? GetTimeStoNS(1.0 / freqHz) : 0; }

	/// <summary>
	/// Simple high-resolution timer class
	/// </summary>
	class Stopwatch
	{
	public:
		using Clock = std::chrono::high_resolution_clock;
		using Duration = std::chrono::duration<slong, std::nano>;
		using TimePoint = Clock::time_point;

		Stopwatch();

		/// <summary>
		/// Start or resume timer
		/// </summary>
		void Start(slong offsetNS = 0ull);

		/// <summary>
		/// Reset and restart timer
		/// </summary>
		void Restart(slong offsetNS = 0ull);

		/// <summary>
		/// Stop timer
		/// </summary>
		void Stop();

		/// <summary>
		/// Reset time
		/// </summary>
		void Reset(slong offsetNS = 0ull);

		/// <summary>
		/// Returns true if the stopwatch is rnning
		/// </summary>
		bool GetIsRunning() const;

		/// <summary>
		/// Returns elapsed time in seconds
		/// </summary>
		double GetElapsedS() const;

		/// <summary>
		/// Returns elapsed time in milliseconds
		/// </summary>
		double GetElapsedMS() const;

		/// <summary>
		/// Returns elapsed time in nanoseconds
		/// </summary>
		slong GetElapsedNS() const;

	private:
		mutable Duration elapsedTime;
		TimePoint reference;
		bool isRunning;
	};
}