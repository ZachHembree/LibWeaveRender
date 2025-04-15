#pragma once
#include "IDrawable.hpp"
#include "Resources/VertexBuffer.hpp"
#include "Resources/IndexBuffer.hpp"
#include <math.h>

namespace Weave::D3D11
{
	class Device;
	class Context;
	class Material;

	/// <summary>
	/// Contains vertex and index data needed to instantiate a new mesh
	/// </summary>
	template<typename Vert_T>
	struct MeshDef
	{
		UniqueArray<Vert_T> vertices;
		UniqueArray<USHORT> indices;

		MeshDef(UniqueArray<Vert_T>&& vertices, UniqueArray<USHORT>&& indices) :
			vertices(std::move(vertices)),
			indices(std::move(indices))
		{ }
	};

	/// <summary>
	/// Contains an instance of a drawable mesh
	/// </summary>
	class Mesh : public IDrawable
	{
	public:
		MAKE_MOVE_ONLY(Mesh)

		template <typename Vert_T>
		Mesh(
			Device& device,
			const MeshDef<Vert_T>& def
		) :
			Mesh(device, def.indices, def.vertices.GetData(), def.vertices.GetLength(), sizeof(Vert_T))
		{ }

		template <typename Vert_T>
		Mesh(
			Device& device,
			const IDynamicArray<Vert_T>& vertices,
			const IDynamicArray<USHORT>& indices
		) : 
			Mesh(device, indices, vertices.GetData(), vertices.GetLength(), sizeof(Vert_T))
		{ }

		Mesh(
			Device& device,
			const IDynamicArray<USHORT>& indices,
			const void* vertices,
			size_t vCount,
			size_t vStride
		);

		Mesh();

		/// <summary>
		/// Updates any resources needed prior to drawing
		/// </summary>
		void Setup(Context& ctx) override;

		/// <summary>
		/// Draws the object
		/// </summary>
		void Draw(Context& ctx, Material& mat) override;

		/// <summary>
		/// Returns the number of indices in the index buffer
		/// </summary>
		UINT GetIndexCount() const;;

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

		vec3 translation;
		vec3 scale;
		fquat rotation;
	};
};