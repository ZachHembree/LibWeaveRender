#pragma once
#include "D3D11/IDrawable.hpp"
#include "D3D11/dev/VertexBuffer.hpp"
#include "D3D11/dev/IndexBuffer.hpp"
#include <math.h>

namespace Replica::D3D11
{
	class Device;
	class Context;

	class Mesh : public IDrawable
	{
	public:
		template <typename Vert_t>
		Mesh(
			Device& device,
			const IDynamicCollection<Vert_t>& vertices,
			const IDynamicCollection<USHORT>& indices
		) : 
			vBuf(device, vertices),
			iBuf(device, indices),
			translation(0),
			rotation(1, 0, 0, 0),
			scale(1)
		{ }

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