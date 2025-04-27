#pragma once
#include "WeaveUtils/MinWindow.hpp"
#include "../InternalD3D11.hpp"
#include "DeviceChild.hpp"

namespace Weave::D3D11
{
	/// <summary>
	/// Defines a resolution and associated refresh rates supported by a given DisplayOutput
	/// </summary>
	struct DisplayMode
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
		/// Internal unique identifier
		/// </summary>
		uint id;

		/// <summary>
		/// Returns the refresh rate in cycles per second (Hz)
		/// </summary>
		double GetFramerateHZ(uint index) const
		{
			const uivec2 rat = refreshRates[index];
			return (double)rat.x / (double)rat.y;
		}

		/// <summary>
		/// Internal indices used to uniquely identify a display mode configuration for a given 
		/// DisplayOutput
		/// </summary>
		uivec2 GetID(uint index) const { return uivec2(id, index); }
	};

	/// <summary>
	/// Provides an interface to a DXGI display output
	/// </summary>
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
		/// Returns default monitor config used by the desktop on startup
		/// </summary>
		const WinMonConfig& GetWinDefaults() const;

		/// <summary>
		/// Returns mode ID that most closely aligns with the desktop settings on startup.
		/// Requires a format to be specified first.
		/// </summary>
		uivec2 GetDefaultMode() const { return defaultMode; }

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
		/// Returns the DisplayMode + frequency identifier most closely matching the given resolution 
		/// and refresh rate. Mode ID == (uint)-1 on failure.
		/// </summary>
		uivec2 GetClosestMatch(uivec2 res, uivec2 refresh = uivec2(0)) const;

		/// <summary>
		/// Returns an array of supported resolutions and framerates
		/// </summary>
		const IDynamicArray<DisplayMode>& GetModes() const;

	private:
		string name;
		HMONITOR handle;
		WinMonConfig defaults;

		Formats format;
		ComPtr<IDXGIOutput1> pDisplay;
		UniqueVector<DisplayMode> dispModes;
		uivec2 defaultMode;
	};
}