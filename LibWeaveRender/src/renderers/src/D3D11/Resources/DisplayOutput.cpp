#include "pch.hpp"
#include "WeaveUtils/MinWindow.hpp"
#include "D3D11/Resources/DisplayOutput.hpp"
#include "D3D11/Device.hpp"
#include "D3D11/InternalD3D11.hpp"

using namespace Weave;
using namespace Weave::D3D11;

DisplayOutput::DisplayOutput() : handle(nullptr), format(Formats::UNKNOWN), defaults({}) {}

DisplayOutput::~DisplayOutput() {}

DisplayOutput::DisplayOutput(DisplayOutput&&) noexcept = default;

DisplayOutput& DisplayOutput::operator=(DisplayOutput&&) noexcept = default;

DisplayOutput::DisplayOutput(Device& dev, ComPtr<IDXGIOutput1>&& pOutput, string&& name) :
	DeviceChild(dev),
	pDisplay(std::move(pOutput)),
	name(std::move(name)),
	format(Formats::UNKNOWN),
	defaultMode(-1, -1)
{ 
	DXGI_OUTPUT_DESC desc = {};
	pDisplay->GetDesc(&desc);
	handle = desc.Monitor;

	defaults = MinWindow::GetMonitorConfig(handle);
}

IDXGIOutput1* DisplayOutput::Get() const { return pDisplay.Get(); }

void DisplayOutput::SetFormat(Formats format)
{
	if (format == this->format)
		return;

	uint modeCount = 0;
	D3D_ASSERT_HR(pDisplay->GetDisplayModeList1((DXGI_FORMAT)format, 0, &modeCount, nullptr));

	UniqueVector<DXGI_MODE_DESC1> modes;
	modes.Resize(modeCount);

	D3D_ASSERT_HR(pDisplay->GetDisplayModeList1((DXGI_FORMAT)format, 0, &modeCount, modes.GetData()));

	DisplayMode* pLastMode = nullptr;

	for (const DXGI_MODE_DESC1& mode : modes)
	{
		const bool isProgressive = mode.ScanlineOrdering == DXGI_MODE_SCANLINE_ORDER_PROGRESSIVE ||
			mode.ScanlineOrdering == DXGI_MODE_SCALING_UNSPECIFIED;

		if (isProgressive)
		{
			const uivec2 newRes = uivec2(mode.Width, mode.Height);

			if (pLastMode == nullptr || pLastMode->resolution != newRes)
			{
				const uint id = (uint)dispModes.GetLength();
				pLastMode = &dispModes.EmplaceBack(DisplayMode
				{
					.resolution = newRes,
					.id = id
				});
			}

			const uivec2 newRefresh = uivec2(mode.RefreshRate.Numerator, mode.RefreshRate.Denominator);

			if (pLastMode->refreshRates.IsEmpty() || newRefresh != pLastMode->refreshRates.GetBack())
				pLastMode->refreshRates.Add(newRefresh);
		}
	}

	defaultMode = GetClosestMatch(defaults.res, uivec2(defaults.refreshHz, 1u));
}

Formats DisplayOutput::GetFormat() const { return format; }

uivec2 DisplayOutput::GetClosestMatch(uivec2 res, uivec2 refresh) const
{
	uint closestRes = (uint)-1;
	uint resErr = (uint)-1;

	// Find closest matching resolution by minimizing area error
	for (uint i = 0; i < (uint)dispModes.GetLength(); i++)
	{
		const ivec2 resDelta = ivec2(dispModes[i].resolution) - ivec2(res);;
		const uint newErr = (uint)std::abs(resDelta.x * resDelta.y);

		if (newErr < resErr)
		{
			closestRes = i;
			resErr = newErr;
		}
	}

	const IDynamicArray<uivec2>& refreshRates = dispModes[closestRes].refreshRates;
	const uint defaultRefresh = (uint)refreshRates.GetLength() - 1;

	if (closestRes == (uint)-1) // Match failed
		return uivec2((uint)-1);
	else if (refresh.y == 0) // Use default (highest) refresh rate
		return uivec2(closestRes, defaultRefresh);

	// Find closest refresh by minimizing frequency error in Hz
	uint closestRefresh = (uint)-1;
	const double targetRefreshHz = refresh.x / refresh.y;
	double lastRefreshErr = std::numeric_limits<double>::max();

	for (uint i = 0; i < (uint)refreshRates.GetLength(); i++)
	{
		const double modeRefreshHz = refreshRates[i].x / refreshRates[i].y;
		double newErr = std::abs(modeRefreshHz - targetRefreshHz);

		if (newErr < lastRefreshErr)
		{
			closestRefresh = i;
			lastRefreshErr = newErr;
		}
	}

	if (closestRefresh != (uint)-1) // Match found
		return uivec2(closestRes, closestRefresh);
	else // Match failed - use default (highest) refresh rate
		return uivec2(closestRes, defaultRefresh);
}

string_view DisplayOutput::GetName() const { return name; }

HMONITOR DisplayOutput::GetHandle() const { return handle; }

const WndMonConfig& DisplayOutput::GetWinDefaults() const { return defaults; }

const IDynamicArray<DisplayMode>& DisplayOutput::GetModes() const { return dispModes; }

