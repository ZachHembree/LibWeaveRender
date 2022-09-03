#pragma once
#include "D3D11/IDrawable.hpp"
#include "D3D11/Resources/VertexBuffer.hpp"
#include "D3D11/Resources/IndexBuffer.hpp"
#include <math.h>

namespace Replica::D3D11
{
	class Device;
	class Context;

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
	class Mesh : public IDrawable, public MoveOnlyObjBase
	{
	public:
		template <typename Vert_T>
		Mesh(
			Device& device,
			const MeshDef<Vert_T>& def
		) :
			vBuf(device, def.vertices),
			iBuf(device, def.indices),
			translation(0),
			rotation(1, 0, 0, 0),
			scale(1)
		{ }

		template <typename Vert_T>
		Mesh(
			Device& device,
			const IDynamicCollection<Vert_T>& vertices,
			const IDynamicCollection<USHORT>& indices
		) : 
			vBuf(device, vertices),
			iBuf(device, indices),
			translation(0),
			rotation(1, 0, 0, 0),
			scale(1)
		{ }

		Mesh() : translation(0), rotation(0, 0, 0, 0), scale(0) {}

		Mesh(Mesh&&) = default;
		Mesh& operator=(Mesh&&) = default;

		/// <summary>
		/// Updates any resources needed prior to drawing
		/// </summary>
		void Setup(Context& ctx) override;

		/// <summary>
		/// Draws the object
		/// </summary>
		void Draw(Context& ctx) override;

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