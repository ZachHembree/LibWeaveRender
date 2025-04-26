#include "pch.hpp"
#include "D3D11/Resources/DisplayOutput.hpp"
#include "D3D11/Device.hpp"

using namespace Weave;
using namespace Weave::D3D11;

DisplayOutput::DisplayOutput() : handle(nullptr), format(Formats::UNKNOWN) {}

DisplayOutput::~DisplayOutput() {}

DisplayOutput::DisplayOutput(DisplayOutput&&) noexcept = default;

DisplayOutput& DisplayOutput::operator=(DisplayOutput&&) noexcept = default;

DisplayOutput::DisplayOutput(Device& dev, ComPtr<IDXGIOutput1>&& pOutput, string&& name) :
	DeviceChild(dev),
	pDisplay(std::move(pOutput)),
	name(std::move(name))
{ 
	DXGI_OUTPUT_DESC desc = {};
	pDisplay->GetDesc(&desc);
	handle = desc.Monitor;
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

	DisplayModes* pLastMode = nullptr;

	for (const DXGI_MODE_DESC1& mode : modes)
	{
		const bool isProgressive = mode.ScanlineOrdering == DXGI_MODE_SCANLINE_ORDER_PROGRESSIVE ||
			mode.ScanlineOrdering == DXGI_MODE_SCALING_UNSPECIFIED;

		if (isProgressive)
		{
			const uivec2 newRes = uivec2(mode.Width, mode.Height);

			if (pLastMode == nullptr || pLastMode->resolution != newRes)
			{
				pLastMode = &dispModes.EmplaceBack(DisplayModes{});
				pLastMode->resolution = newRes;
				pLastMode->format = (Formats)mode.Format;
			}

			const uivec2 newRefresh = uivec2(mode.RefreshRate.Numerator, mode.RefreshRate.Denominator);

			if (pLastMode->refreshRates.IsEmpty() || newRefresh != pLastMode->refreshRates.GetBack())
				pLastMode->refreshRates.Add(newRefresh);
		}
	}
}

Formats DisplayOutput::GetFormat() const { return format; }

string_view DisplayOutput::GetName() const { return name; }

HMONITOR DisplayOutput::GetHandle() const { return handle; }

const IDynamicArray<DisplayModes>& DisplayOutput::GetModes() const { return dispModes; }

