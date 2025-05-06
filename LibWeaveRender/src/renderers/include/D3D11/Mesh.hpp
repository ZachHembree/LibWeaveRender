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