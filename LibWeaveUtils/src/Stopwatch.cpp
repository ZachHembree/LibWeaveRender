#include "pch.hpp"
#include "WeaveUtils/Stopwatch.hpp"

using namespace Weave;

Stopwatch::Stopwatch() :
	isRunning(false),
	elapsedTime(0)
{ }

/// <summary>
/// Start or resume timer
/// </summary>
void Stopwatch::Start(slong offsetNS)
{
	if (!isRunning)
	{
		reference = Clock::now() - elapsedTime - Duration(offsetNS);
		elapsedTime = Clock::now() - reference;
		isRunning = true;
	}
}

/// <summary>
/// Reset and restart timer
/// </summary>
void Stopwatch::Restart(slong offsetNS)
{
	Reset(offsetNS);
	isRunning = true;
}

/// <summary>
/// Stop timer
/// </summary>
void Stopwatch::Stop()
{
	if (isRunning)
	{
		elapsedTime = Clock::now() - reference;
		reference = Clock::now();
		isRunning = false;
	}
}

/// <summary>
/// Reset time
/// </summary>
void Stopwatch::Reset(slong offsetNS)
{
	reference = Clock::now() - Duration(offsetNS);
	elapsedTime = Duration(0);
}

/// <summary>
/// Returns true if the stopwatch is rnning
/// </summary>
bool Stopwatch::GetIsRunning() const { return isRunning; }

/// <summary>
/// Returns elapsed time in seconds
/// </summary>
double Stopwatch::GetElapsedS() const
{
	return GetElapsedMS() / 1E3;
}

/// <summary>
/// Returns elapsed time in milliseconds
/// </summary>
double Stopwatch::GetElapsedMS() const
{
	return (double)GetElapsedNS() / 1E6;
}

/// <summary>
/// Returns elapsed time in nanoseconds
/// </summary>
slong Stopwatch::GetElapsedNS() const
{
	if (isRunning)
		elapsedTime = Clock::now() - reference;

	return elapsedTime.count();
}
