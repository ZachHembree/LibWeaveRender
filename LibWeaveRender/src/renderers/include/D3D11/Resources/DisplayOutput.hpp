#pragma once
#include "../InternalD3D11.hpp"
#include "DeviceChild.hpp"

namespace Weave::D3D11
{
	struct DisplayModes
	{
		/// <summary>
		/// Resolution supported by the display mode. x == width. y == height.
		/// </summary>
		uivec2 resolution;

		/// <summary>
		/// Supported refresh rates as a rational number. x == numerator. y == denominator.
		/// </summary>
		UniqueVector<uivec2> refreshRates;

		/// <summary>
		/// Supported format
		/// </summary>
		Formats format;

		/// <summary>
		/// Returns the refresh rate in cycles per second (Hz)
		/// </summary>
		double GetFramerateHZ(uint index) const
		{
			const uivec2 rat = refreshRates[index];
			return (double)rat.x / (double)rat.y;
		}
	};

	class DisplayOutput : public DeviceChild
	{
	public:
		DisplayOutput();

		DisplayOutput(Device& dev, ComPtr<IDXGIOutput1>&& pOutput, string&& name);

		~DisplayOutput();

		DisplayOutput(DisplayOutput&&) noexcept;

		DisplayOutput& operator=(DisplayOutput&&) noexcept;

		/// <summary>
		/// Returns a pointer to the underlying DXGI object
		/// </summary>
		IDXGIOutput1* Get() const;

		/// <summary>
		/// Returns a user-friendly name for the outptu
		/// </summary>
		string_view GetName() const;
		
		/// <summary>
		/// Returns a unique Win32 identifier for the monitor
		/// </summary>
		HMONITOR GetHandle() const;

		/// <summary>
		/// Initialzies or changes the display output to the given format and updates supported modes.
		/// Unsupported formats will result in an empty mode list.
		/// </summary>
		void SetFormat(Formats format);

		/// <summary>
		/// Returns the format currently used by the output
		/// </summary>
		Formats GetFormat() const;

		/// <summary>
		/// Returns an array of supported resolutions and framerates
		/// </summary>
		const IDynamicArray<DisplayModes>& GetModes() const;

	private:
		string name;
		HMONITOR handle;
		Formats format;
		ComPtr<IDXGIOutput1> pDisplay;
		UniqueVector<DisplayModes> dispModes;
	};
}