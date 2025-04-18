#pragma once
#include "WeaveEffects/ShaderDataHandles.hpp"
#include "../Resources/DeviceChild.hpp"
#include "../Resources/ConstantBuffer.hpp"
#include "../Resources/ConstantGroupMap.hpp"
#include "../Resources/ResourceMap.hpp"

namespace Weave::D3D11
{
	using Effects::ShaderDef;
	using Effects::ShaderDefHandle;

	class ShaderLibrary;

	/// <summary>
	/// Abstract base class used internally for shading stage subtypes, represents an individual compiled variant and
	/// relevant resource mappings.
	/// </summary>
	class ShaderVariantBase : public DeviceChild
	{
	public:
		MAKE_NO_COPY(ShaderVariantBase)

		/// <summary>
		/// Returns unique ID representing the entrypoint name in the associated ShaderLibrary
		/// </summary>
		uint GetNameID() const;

		/// <summary>
		/// Returns handle to preprocessed definition
		/// </summary>
		ShaderDefHandle GetDefinition() const;

		/// <summary>
		/// Returns a pointer to the underlying compiled shader object
		/// </summary>
		virtual ID3D11DeviceChild* Get() const = 0;

		/// <summary>
		/// Returns an enum indicating the shader's subtype.
		/// </summary>
		virtual ShadeStages GetStage() const = 0;

		/// <summary>
		/// Returns map representing the size and layout of constants used by the shader. Returns nullptr if undefined or empty.
		/// </summary>
		const ConstantGroupMap* GetConstantMap() const;

		/// <summary>
		/// Returns an array of buffers used to store constants used by the shader. Returns nullptr if undefined or empty.
		/// </summary>
		IDynamicArray<ConstantBuffer>& GetConstantBuffers() const;

		/// <summary>
		/// Returns the layout map for shader resource views. Returns nullptr if undefined or empty.
		/// </summary>
		const ResourceViewMap* GetResViewMap() const;

		/// <summary>
		/// Returns the layout map for sampler states. Returns nullptr if undefined or empty.
		/// </summary>
		const SamplerMap* GetSampMap() const;

		/// <summary>
		/// Returns the layout map for unordered access views. Returns nullptr if undefined or empty.
		/// </summary>
		virtual const UnorderedAccessMap* GetUAVMap() const;

	protected:
		ShaderDefHandle def;

		std::unique_ptr<ConstantGroupMap> pConstants;
		std::unique_ptr<SamplerMap> pSampMap;
		std::unique_ptr<ResourceViewMap> pSrvMap;
		mutable UniqueArray<ConstantBuffer> cbufs;

		ShaderVariantBase();

		ShaderVariantBase(Device& dev, const ShaderDefHandle& def);

		ShaderVariantBase(ShaderVariantBase&& other) noexcept;

		ShaderVariantBase& operator=(ShaderVariantBase&& other) noexcept;
	};
}
