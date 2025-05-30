#pragma once
#include "WeaveUtils/GlobalUtils.hpp"
#include "../D3D11Utils.hpp"

namespace Weave::D3D11
{
	class Device;
	class Renderer;

	/// <summary>
	/// Base class for types representing interfaces to device resources
	/// </summary>
	class DeviceChild
	{
	public:
		/// <summary>
		/// Returns the device associated with the child object
		/// </summary>
		Device& GetDevice() const;

		/// <summary>
		/// Returns reference to renderer using this object
		/// </summary>
		Renderer& GetRenderer() const;

		/// <summary>
		/// Returns true if the object is valid and has been initialized
		/// </summary>
		virtual bool GetIsValid() const;

	protected:
		DECL_MOVE_ONLY(DeviceChild);

		Device* pDev;

		DeviceChild();

		DeviceChild(Device& dev);

		virtual ~DeviceChild();
	};
}