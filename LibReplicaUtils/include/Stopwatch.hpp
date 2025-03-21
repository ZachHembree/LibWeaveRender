#pragma once
#include <chrono>
#include "ReplicaInt.hpp"

namespace Replica
{
	/// <summary>
	/// Simple high-resolution timer class
	/// </summary>
	class Stopwatch
	{
		using Clock = std::chrono::high_resolution_clock;
		using Duration = std::chrono::duration<slong, std::nano>;
		using TimePoint = std::chrono::high_resolution_clock::time_point;

	public:
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