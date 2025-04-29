#pragma once
#include <algorithm>
#include "DynamicCollections.hpp"

namespace Weave
{
	/// <summary>
	/// Class template for recording statistics using scalar numeric values in a sliding window
	/// </summary>
	template <typename T> requires (std::integral<T> || std::floating_point<T>) && std::is_trivial_v<T>
	class StatsRecorder
	{
	public:
		MAKE_MOVE_ONLY(StatsRecorder)

		StatsRecorder(size_t windowSize) :
			values(windowSize, 0),
			sortedValues(windowSize),
			index(0),
			wndSize(0),
			sum(0),
			isSorted(false)
		{ }

		/// <summary>
		/// Clears the recorder
		/// </summary>
		void Clear()
		{
			SetArrNull(values);
			wndSize = 0;
			index = 0;
			sum = 0;
			isSorted = false;
		}

		/// <summary>
		/// Updates and adds a new value to the recorder
		/// </summary>
		void AddValue(T value)
		{
			WV_ASSERT(values.GetLength() > 0);

			sum -= values[index];
			values[index] = value;
			sum += value;

			index++;
			wndSize = std::max(wndSize, index);
			index %= values.GetLength();
			isSorted = false;
		}

		/// <summary>
		/// Returns the maximum capacity of the recorder
		/// </summary>
		size_t GetMaxWindowSize() const { return values.GetLength(); }

		/// <summary>
		/// Returns the window size. May be less than max if the recorder hasn't had 
		/// time to fully populate
		/// </summary>
		size_t GetWindowSize() const { return wndSize; }

		/// <summary>
		/// Returns value of the nearest percentile on a normalized scale on [0, 1]
		/// </summary>
		T GetPercentile(double pct) const
		{
			WV_ASSERT(wndSize > 0);

			if (!isSorted)
			{
				ArrMemCopy(sortedValues, values);
				std::sort(sortedValues.GetData(), sortedValues.GetData() + GetWindowSize());
				isSorted = true;
			}

			pct = std::clamp(pct, 0.0, 1.0);
			return sortedValues[(ptrdiff_t)std::round(pct * (double)(wndSize - 1))];
		}

		/// <summary>
		/// Returns the mean value
		/// </summary>
		T GetAverage() const { WV_ASSERT(wndSize > 0); return sum / static_cast<T>(wndSize); }

	private:
		mutable bool isSorted;
		mutable UniqueArray<T> sortedValues;
		UniqueArray<T> values;
		size_t index, wndSize;
		T sum;
	};
}