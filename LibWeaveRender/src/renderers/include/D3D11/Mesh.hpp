#pragma once
#include "IDrawable.hpp"
#include "Resources/VertexBuffer.hpp"
#include "Resources/IndexBuffer.hpp"

namespace Weave::D3D11
{
	class Device;
	class CtxBase;
	class Material;

	/// <summary>
	/// Determines how vertex topology is interpreted by the input assembler
	/// </summary>
	enum class PrimTopology
	{
		UNDEFINED = D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED,
		POINTLIST = D3D11_PRIMITIVE_TOPOLOGY_POINTLIST,
		LINELIST = D3D11_PRIMITIVE_TOPOLOGY_LINELIST,
		LINESTRIP = D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP,
		TRIANGLELIST = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
		TRIANGLESTRIP = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP,
		LINELIST_ADJ = D3D11_PRIMITIVE_TOPOLOGY_LINELIST_ADJ,
		LINESTRIP_ADJ = D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP_ADJ,
		TRIANGLELIST_ADJ = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST_ADJ,
		TRIANGLESTRIP_ADJ = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP_ADJ,
		CP1_PATCHLIST = D3D11_PRIMITIVE_TOPOLOGY_1_CONTROL_POINT_PATCHLIST,
		CP2_PATCHLIST = D3D11_PRIMITIVE_TOPOLOGY_2_CONTROL_POINT_PATCHLIST,
		CP3_PATCHLIST = D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST,
		CP4_PATCHLIST = D3D11_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST,
		CP5_PATCHLIST = D3D11_PRIMITIVE_TOPOLOGY_5_CONTROL_POINT_PATCHLIST,
		CP6_PATCHLIST = D3D11_PRIMITIVE_TOPOLOGY_6_CONTROL_POINT_PATCHLIST,
		CP7_PATCHLIST = D3D11_PRIMITIVE_TOPOLOGY_7_CONTROL_POINT_PATCHLIST,
		CP8_PATCHLIST = D3D11_PRIMITIVE_TOPOLOGY_8_CONTROL_POINT_PATCHLIST,
		CP9_PATCHLIST = D3D11_PRIMITIVE_TOPOLOGY_9_CONTROL_POINT_PATCHLIST,
		CP10_PATCHLIST = D3D11_PRIMITIVE_TOPOLOGY_10_CONTROL_POINT_PATCHLIST,
		CP11_PATCHLIST = D3D11_PRIMITIVE_TOPOLOGY_11_CONTROL_POINT_PATCHLIST,
		CP12_PATCHLIST = D3D11_PRIMITIVE_TOPOLOGY_12_CONTROL_POINT_PATCHLIST,
		CP13_PATCHLIST = D3D11_PRIMITIVE_TOPOLOGY_13_CONTROL_POINT_PATCHLIST,
		CP14_PATCHLIST = D3D11_PRIMITIVE_TOPOLOGY_14_CONTROL_POINT_PATCHLIST,
		CP15_PATCHLIST = D3D11_PRIMITIVE_TOPOLOGY_15_CONTROL_POINT_PATCHLIST,
		CP16_PATCHLIST = D3D11_PRIMITIVE_TOPOLOGY_16_CONTROL_POINT_PATCHLIST,
		CP17_PATCHLIST = D3D11_PRIMITIVE_TOPOLOGY_17_CONTROL_POINT_PATCHLIST,
		CP18_PATCHLIST = D3D11_PRIMITIVE_TOPOLOGY_18_CONTROL_POINT_PATCHLIST,
		CP19_PATCHLIST = D3D11_PRIMITIVE_TOPOLOGY_19_CONTROL_POINT_PATCHLIST,
		CP20_PATCHLIST = D3D11_PRIMITIVE_TOPOLOGY_20_CONTROL_POINT_PATCHLIST,
		CP21_PATCHLIST = D3D11_PRIMITIVE_TOPOLOGY_21_CONTROL_POINT_PATCHLIST,
		CP22_PATCHLIST = D3D11_PRIMITIVE_TOPOLOGY_22_CONTROL_POINT_PATCHLIST,
		CP23_PATCHLIST = D3D11_PRIMITIVE_TOPOLOGY_23_CONTROL_POINT_PATCHLIST,
		CP24_PATCHLIST = D3D11_PRIMITIVE_TOPOLOGY_24_CONTROL_POINT_PATCHLIST,
		CP25_PATCHLIST = D3D11_PRIMITIVE_TOPOLOGY_25_CONTROL_POINT_PATCHLIST,
		CP26_PATCHLIST = D3D11_PRIMITIVE_TOPOLOGY_26_CONTROL_POINT_PATCHLIST,
		CP27_PATCHLIST = D3D11_PRIMITIVE_TOPOLOGY_27_CONTROL_POINT_PATCHLIST,
		CP28_PATCHLIST = D3D11_PRIMITIVE_TOPOLOGY_28_CONTROL_POINT_PATCHLIST,
		CP29_PATCHLIST = D3D11_PRIMITIVE_TOPOLOGY_29_CONTROL_POINT_PATCHLIST,
		CP30_PATCHLIST = D3D11_PRIMITIVE_TOPOLOGY_30_CONTROL_POINT_PATCHLIST,
		CP31_PATCHLIST = D3D11_PRIMITIVE_TOPOLOGY_31_CONTROL_POINT_PATCHLIST,
		CP32_PATCHLIST = D3D11_PRIMITIVE_TOPOLOGY_32_CONTROL_POINT_PATCHLIST
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
			Mesh(device, def.indices, def.vertices.GetData(), def.vertices.GetLength(), sizeof(VertT), topology)
		{ }

		template <typename VertT, typename IndexT = ushort>
		Mesh(
			Device& device,
			const IDynamicArray<VertT>& vertices,
			const IDynamicArray<IndexT>& indices, 
			PrimTopology topology = PrimTopology::TRIANGLELIST
		) : 
			Mesh(device, indices, vertices.GetData(), vertices.GetLength(), sizeof(VertT), topology)
		{ }

		Mesh(
			Device& device,
			const IDynamicArray<ushort>& indices,
			const void* vertices,
			size_t vCount,
			size_t vStride, 
			PrimTopology topology = PrimTopology::TRIANGLELIST
		);

		Mesh(
			Device& device,
			const IDynamicArray<uint>& indices,
			const void* vertices,
			size_t vCount,
			size_t vStride, 
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