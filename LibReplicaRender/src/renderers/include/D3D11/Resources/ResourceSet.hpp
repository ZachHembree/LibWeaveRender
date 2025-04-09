#pragma once
#include "ReplicaUtils/Span.hpp"
#include "ResourceBase.hpp"
#include <unordered_map>

namespace Replica::D3D11
{
	class ConstantGroupMap;

	/// <summary>
	/// A reusable variable-length map for associating arbitrary resources with an Effect or Shader
	/// </summary>
	class ResourceSet
	{
	public:
		MAKE_NO_COPY(ResourceSet)

		/// <summary>
		/// Stores a pointer to a resource view paired with a stringID
		/// </summary>
		template<typename T>
		struct ResView
		{
			uint stringID;
			T* pView;
		};

		/// <summary>
		/// Stores mappings for string ID <-> view lookup
		/// </summary>
		template<typename T>
		struct ViewMap
		{
			UniqueVector<ResView<T>> data;
			// stringID -> index
			std::unordered_map<uint, uint> stringViewMap;

			/// <summary>
			/// Updates or adds the resource with the given string ID
			/// </summary>
			void SetResource(uint stringID, T* pValue)
			{
				const auto& it = stringViewMap.find(stringID);

				if (it != stringViewMap.end())
				{
					data[it->second].pView = pValue;
				}
				else
				{
					stringViewMap.emplace(stringID, (uint)data.GetLength());
					data.emplace_back(stringID, pValue);
				}
			}

			/// <summary>
			/// Resets the collection
			/// </summary>
			void Clear()
			{
				data.clear();
				stringViewMap.clear();
			}
		};

		struct Constant
		{
			uint stringID;
			uint size;
			// Position in byte vector
			uint offset;
		};

		struct ConstantGroup
		{
			UniqueVector<byte> data;
			// Const data layout
			UniqueVector<Constant> constants;
			// stringID -> const Index
			std::unordered_map<uint, uint> stringConstMap;

			void SetValue(uint stringID, const Span<byte>& newValue);

			void Clear();
		};

		ResourceSet();

		ResourceSet(ResourceSet&&) noexcept;

		ResourceSet& operator=(ResourceSet&&) noexcept;

		void SetSampler(uint stringID, const ComPtr<ID3D11SamplerState>& pSamp);

		void SetSRV(uint stringID, const ComPtr<ID3D11ShaderResourceView>& pSRV);

		void SetUAV(uint stringID, const ComPtr<ID3D11UnorderedAccessView>& pUAV);

		void SetConstant(uint stringID, const Span<byte>& newValue);

		template<typename T>
		void SetConstant(uint stringID, const T& value) { constants.SetValue(stringID, {(byte*)(&value), sizeof(T)}); }

		const IDynamicArray<ResView<ID3D11SamplerState>>& GetSamplers() const;

		const IDynamicArray<ResView<ID3D11ShaderResourceView>>& GetSRVs() const;

		const IDynamicArray<ResView<ID3D11UnorderedAccessView>>& GetUAVs() const;

		const IDynamicArray<Span<byte>>& GetMappedConstants(const ConstantGroupMap& map) const;

		void Clear()
		{
			constants.Clear();
			sampMap.Clear();
			textures.Clear();
			rwTextures.Clear();
		}

	private:	
		ConstantGroup constants;
		ViewMap<ID3D11SamplerState> sampMap;
		ViewMap<ID3D11ShaderResourceView> textures;
		ViewMap<ID3D11UnorderedAccessView> rwTextures;
	};
}