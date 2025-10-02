#pragma once
#include "WeaveEffects/ShaderData.hpp"
#include <optional>

namespace Weave::Effects
{
	class ShaderRegistryMap;

	/// <summary>
	/// Provides access to unique constant buffer definition and provides array-like member access
	/// </summary>
	class ConstBufDefHandle
	{
	public:
		ConstBufDefHandle();

		ConstBufDefHandle(const ShaderRegistryMap& map, uint bufID);

		/// <summary>
		/// Returns string ID associated with the constant buffer in the parent library
		/// </summary>
		uint GetNameID() const;

		/// <summary>
		/// Returns the name of the constant buffer
		/// </summary>
		string_view GetName() const;

		/// <summary>
		/// Returns total buffer size in bytes
		/// </summary>
		uint GetSize() const;

		/// <summary>
		/// Returns the constant definition at the given index
		/// </summary>
		const ConstDef& operator[](ptrdiff_t index) const;

		/// <summary>
		/// Returns the total number of constants in the buffer
		/// </summary>
		size_t GetLength() const;

		/// <summary>
		/// Map for stringID lookup
		/// </summary>
		const IStringIDMap& GetStringMap() const;

	private:
		const ShaderRegistryMap* pMap;
		const ConstBufDef* pDef;
	};

	/// <summary>
	/// Provides access to the input/output signature of a shader for a given stage
	/// </summary>
	class IOLayoutHandle
	{
	public:
		IOLayoutHandle();
		
		IOLayoutHandle(const ShaderRegistryMap& map, uint layoutID);

		/// <summary>
		/// Returns the IO member at the given index
		/// </summary>
		const IOElementDef& operator[](const ptrdiff_t index) const;

		/// <summary>
		/// Returns the total number of members/params for a given stage IO
		/// </summary>
		size_t GetLength() const;

		/// <summary>
		/// Map for stringID lookup
		/// </summary>
		const IStringIDMap& GetStringMap() const;

	private:
		const ShaderRegistryMap* pMap;
		IDSpan layout;
	};

	/// <summary>
	/// Provides access to a group of resources used by a shader or effect 
	/// </summary>
	class ResourceGroupHandle
	{
	public:
		ResourceGroupHandle();
		
		ResourceGroupHandle(const ShaderRegistryMap& map, uint layoutID);

		/// <summary>
		/// Returns the resource definition at the given index.
		/// </summary>
		const ResourceDef& operator[](const ptrdiff_t index) const;

		/// <summary>
		/// Returns the total number of resources in the group
		/// </summary>
		size_t GetLength() const;

		/// <summary>
		/// Map for stringID lookup
		/// </summary>
		const IStringIDMap& GetStringMap() const;

	private:
		const ShaderRegistryMap* pMap;
		IDSpan layout;
	};

	/// <summary>
	/// Provides access to a group of constant buffers used by a shader
	/// </summary>
	class ConstBufGroupHandle
	{
	public:
		ConstBufGroupHandle();

		ConstBufGroupHandle(const ShaderRegistryMap& map, uint groupID);

		/// <summary>
		/// Returns the buffer handle for the given index
		/// </summary>
		ConstBufDefHandle operator[](const ptrdiff_t index) const;

		/// <summary>
		/// Returns the total number of buffers in the group
		/// </summary>
		size_t GetLength() const;

		/// <summary>
		/// Map for stringID lookup
		/// </summary>
		const IStringIDMap& GetStringMap() const;

	private:
		const ShaderRegistryMap* pMap;
		IDSpan layout;
	};

	/// <summary>
	/// Provides access to a shader's definition and associated resources
	/// </summary>
	class ShaderDefHandle
	{
	public:
		ShaderDefHandle();

		ShaderDefHandle(const ShaderRegistryMap& map, uint shaderID);

		/// <summary>
		/// Returns the string ID associated with the file the shader was compiled from
		/// </summary>
		uint GetFilePathID() const;

		/// <summary>
		/// Returns the file path the shader was compiled from
		/// </summary>
		string_view GetFilePath() const;

		/// <summary>
		/// Returns the string ID associated with the shader's name
		/// </summary>
		uint GetNameID() const;

		/// <summary>
		/// Returns the name of the shader
		/// </summary>
		string_view GetName() const;

		/// <summary>
		/// Returns precompiled platform-specific bytecode
		/// </summary>
		ByteSpan GetBinSrc() const;

		/// <summary>
		/// Identifies the shading stage defined by the shader
		/// </summary>
		ShadeStages GetStage() const;

		/// <summary>
		/// Returns thread group size for compute shaders
		/// </summary>
		tvec3<uint> GetThreadGroupSize() const;

		/// <summary>
		/// Returns the input layout/signature for the shader
		/// </summary>
		std::optional<IOLayoutHandle> GetInLayout() const;

		/// <summary>
		/// Returns the output/return signature for the shader
		/// </summary>
		std::optional<IOLayoutHandle> GetOutLayout() const;

		/// <summary>
		/// Returns the unique set of resources required by the shader
		/// </summary>
		std::optional<ResourceGroupHandle> GetResources() const;

		/// <summary>
		/// Returns the constant buffers used by the shader
		/// </summary>
		std::optional<ConstBufGroupHandle> GetConstantBuffers() const;

		/// <summary>
		/// Returns the raw shader definition
		/// </summary>
		const ShaderDef& GetDefinition() const;

		/// <summary>
		/// Returns the parent registry backing the handle
		/// </summary>
		const ShaderRegistryMap& GetRegistry() const;

		/// <summary>
		/// Map for stringID lookup
		/// </summary>
		const IStringIDMap& GetStringMap() const;

	private:
		const ShaderRegistryMap* pMap;
		const ShaderDef* pDef;
	};

	/// <summary>
	/// Provides access to an effect's definition
	/// </summary>
	class EffectDefHandle
	{
	public:
		MAKE_DEF_MOVE_COPY(EffectDefHandle);

		EffectDefHandle();

		EffectDefHandle(const ShaderRegistryMap& map, uint effectID);

		/// <summary>
		/// Returns the string ID associated with the effect's name
		/// </summary>
		uint GetNameID() const;

		/// <summary>
		/// Returns the name of the effect
		/// </summary>
		string_view GetName() const;

		/// <summary>
		/// Returns a shader definition used by the effect for the given pass and index
		/// </summary>
		ShaderDefHandle GetShader(int pass, int shader) const;

		/// <summary>
		/// Returns shaders for the given pass
		/// </summary>
		IDSpan GetPass(int pass) const;

		/// <summary>
		/// Returns the number of shaders used by the given pass
		/// </summary>
		uint GetShaderCount(int pass) const;

		/// <summary>
		/// Returns the total number of passes
		/// </summary>
		uint GetPassCount() const;

		/// <summary>
		/// Returns raw effect definition
		/// </summary>
		const EffectDef& GetDefinition() const;

		/// <summary>
		/// Returns the parent registry backing the handle
		/// </summary>
		const ShaderRegistryMap& GetRegistry() const;

		/// <summary>
		/// Map for stringID lookup
		/// </summary>
		const IStringIDMap& GetStringMap() const;

	private:
		EffectDef def;
		const ShaderRegistryMap* pMap;
		IDSpan passes;
	};
}