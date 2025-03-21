#pragma once
#include "ShaderData.hpp"

namespace Replica::Effects
{
	class ShaderRegistryMap;

	class ConstBufDefHandle
	{
	public:
		ConstBufDefHandle();

		ConstBufDefHandle(const ShaderRegistryMap& map, uint bufID);

		uint GetNameID() const;

		uint GetSize() const;

		const ConstDef& GetConstant(int index) const;

		uint GetConstantCount() const;

	private:
		const ShaderRegistryMap* pMap;
		const ConstBufDef* pDef;
	};

	class ShaderDefHandle
	{
	public:
		ShaderDefHandle();

		ShaderDefHandle(const ShaderRegistryMap& map, uint shaderID);

		uint GetFilePathID() const;

		uint GetNameID() const;

		const IDynamicArray<byte>& GetBinSrc() const;

		ShadeStages GetStage() const;

		tvec3<uint> GetThreadGroupSize() const;

		const IOElementDef& GetInElement(int index) const;

		uint GetInLayoutLength() const;

		const IOElementDef& GetOutElement(int index) const;

		uint GetOutLayoutLength() const;

		const ResourceDef& GetResource(int index) const;

		uint GetResourceCount() const;

		ConstBufDefHandle GetConstantBuffer(int index) const;

		uint GetConstBufCount() const;

	private:
		const ShaderRegistryMap* pMap;
		const ShaderDef* pDef;
	};

	class EffectDefHandle
	{
	public:
		EffectDefHandle();

		EffectDefHandle(const ShaderRegistryMap& map, uint effectID);

		uint GetNameID() const;

		ShaderDefHandle GetShader(int pass, int shader) const;

		uint GetShaderCount(int pass) const;

		uint GetPassCount() const;

	private:
		const ShaderRegistryMap* pMap;
		const EffectDef* pDef;
	};
}