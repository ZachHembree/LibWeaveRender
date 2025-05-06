#pragma once
#include "IDrawable.hpp"
#include "Resources/VertexBuffer.hpp"
#include "Resources/IndexBuffer.hpp"

enum D3D_PRIMITIVE_TOPOLOGY;

namespace Weave::D3D11
{
	class Device;
	class CtxBase;
	class Material;

	/// <summary>
	/// Determines how vertex topology is interpreted by the input assembler
	/// </summary>
	struct PrimTopology : EnumAlias<D3D_PRIMITIVE_TOPOLOGY>
	{
		using EnumAlias::EnumAlias;

		static const PrimTopology UNDEFINED;
		static const PrimTopology POINTLIST;
		static const PrimTopology LINELIST;
		static const PrimTopology LINESTRIP;
		static const PrimTopology TRIANGLELIST;
		static const PrimTopology TRIANGLESTRIP;
		static const PrimTopology LINELIST_ADJ;
		static const PrimTopology LINESTRIP_ADJ;
		static const PrimTopology TRIANGLELIST_ADJ;
		static const PrimTopology TRIANGLESTRIP_ADJ;
		static const PrimTopology CP1_PATCHLIST;
		static const PrimTopology CP2_PATCHLIST;
		static const PrimTopology CP3_PATCHLIST;
		static const PrimTopology CP4_PATCHLIST;
		static const PrimTopology CP5_PATCHLIST;
		static const PrimTopology CP6_PATCHLIST;
		static const PrimTopology CP7_PATCHLIST;
		static const PrimTopology CP8_PATCHLIST;
		static const PrimTopology CP9_PATCHLIST;
		static const PrimTopology CP10_PATCHLIST;
		static const PrimTopology CP11_PATCHLIST;
		static const PrimTopology CP12_PATCHLIST;
		static const PrimTopology CP13_PATCHLIST;
		static const PrimTopology CP14_PATCHLIST;
		static const PrimTopology CP15_PATCHLIST;
		static const PrimTopology CP16_PATCHLIST;
		static const PrimTopology CP17_PATCHLIST;
		static const PrimTopology CP18_PATCHLIST;
		static const PrimTopology CP19_PATCHLIST;
		static const PrimTopology CP20_PATCHLIST;
		static const PrimTopology CP21_PATCHLIST;
		static const PrimTopology CP22_PATCHLIST;
		static const PrimTopology CP23_PATCHLIST;
		static const PrimTopology CP24_PATCHLIST;
		static const PrimTopology CP25_PATCHLIST;
		static const PrimTopology CP26_PATCHLIST;
		static const PrimTopology CP27_PATCHLIST;
		static const PrimTopology CP28_PATCHLIST;
		static const PrimTopology CP29_PATCHLIST;
		static const PrimTopology CP30_PATCHLIST;
		static const PrimTopology CP31_PATCHLIST;
		static const PrimTopology CP32_PATCHLIST;
	};

	/// <summary>
	/// Contains vertex and index data needed to instantiate a new mesh.
	/// Only 16 or 32bit unsigned integers can be used for indices.
	/// </summary>
	template<typename VertT, typename IndexT = ushort>
	struct MeshDef
	{
		UniqueArray<VertT> vertices;
		UniqueArray<IndexT> indices;
		PrimTopology topology;

		MeshDef() : topology(PrimTopology::UNDEFINED)
		{ }

		MeshDef(UniqueArray<VertT>&& vertices, UniqueArray<IndexT>&& indices, PrimTopology topology = PrimTopology::TRIANGLELIST) :
			vertices(std::move(vertices)),
			indices(std::move(indices)),
			topology(topology)
		{ }

		MeshDef(UniqueArray<VertT>&& vertices, PrimTopology topology = PrimTopology::TRIANGLELIST) :
			vertices(std::move(vertices)),
			indices(std::move(indices)),
			topology(topology)
		{ }
	};

	/// <summary>
	/// Contains an instance of a drawable mesh
	/// </summary>
	class Mesh : public IDrawable
	{
	public:
		MAKE_MOVE_ONLY(Mesh)

		template <typename VertT, typename IndexT = ushort>
		Mesh(
			Device& device,
			const MeshDef<VertT, IndexT>& def, 
			PrimTopology topology = PrimTopology::TRIANGLELIST
		) :
			Mesh(device, def.indices, def.vertices.GetData(), (uint)def.vertices.GetLength(), (uint)sizeof(VertT), topology)
		{ }

		template <typename VertT, typename IndexT = ushort>
		Mesh(
			Device& device,
			const IDynamicArray<VertT>& vertices,
			const IDynamicArray<IndexT>& indices, 
			PrimTopology topology = PrimTopology::TRIANGLELIST
		) : 
			Mesh(device, indices, vertices.GetData(), (uint)vertices.GetLength(), (uint)sizeof(VertT), topology)
		{ }

		Mesh(
			Device& device,
			const IDynamicArray<ushort>& indices,
			const void* vertices,
			uint vCount,
			uint vStride,
			PrimTopology topology = PrimTopology::TRIANGLELIST
		);

		Mesh(
			Device& device,
			const IDynamicArray<uint>& indices,
			const void* vertices,
			uint vCount,
			uint vStride,
			PrimTopology topology = PrimTopology::TRIANGLELIST
		);

		Mesh();

		/// <summary>
		/// Retuns the mesh's topology. Triangle list by default.
		/// </summary>
		PrimTopology GetTopology() const;

		/// <summary>
		/// Updates any resources needed prior to drawing
		/// </summary>
		void Setup(CtxBase& ctx) override;

		/// <summary>
		/// Draws the object
		/// </summary>
		void Draw(CtxBase& ctx, Material& mat) override;

		/// <summary>
		/// Returns the number of indices in the index buffer
		/// </summary>
		uint GetIndexCount() const;;

		/// <summary>
		/// Retrieves the model matrix representing the translation, rotation and
		/// scale of the object
		/// </summary>
		mat4 GetModelMatrix();

		/// <summary>
		/// Sets the position or translation of the mesh
		/// </summary>
		void SetTranslation(vec3 translation);

		/// <summary>
		/// Returns the position or translation of the mesh
		/// </summary>
		vec3 GetTranslation();

		/// <summary>
		/// Sets the rotation of the mesh about its center
		/// </summary>
		void SetRotation(fquat rotation);

		/// <summary>
		/// Returns the rotation of the mesh about its center
		/// </summary>
		fquat GetRotation();

		/// <summary>
		/// Returns the size or scale of the mesh
		/// </summary>
		vec3 GetScale();

		/// <summary>
		/// Sets the size or scale of the mesh
		/// </summary>
		void SetScale(vec3 scale);

	protected:
		VertexBuffer vBuf;
		IndexBuffer iBuf;
		PrimTopology topology;

		vec3 translation;
		vec3 scale;
		fquat rotation;
	};
};