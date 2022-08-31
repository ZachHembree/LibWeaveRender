#pragma once
#include "IDrawable.hpp"

namespace Replica::D3D11
{
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
		void Setup(Context& ctx, const mat4& vp) override
		{
			vBuf.Bind(ctx);
			iBuf.Bind(ctx);
		}

		/// <summary>
		/// Draws the object
		/// </summary>
		void Draw(Context& ctx) override
		{
			ctx.DrawIndexed((UINT)iBuf.GetLength());
		}

		/// <summary>
		/// Retrieves the model matrix representing the translation, rotation and
		/// scale of the object
		/// </summary>
		mat4 GetModelMatrix()
		{
			mat4 model = glm::identity<mat4>();
			model *= glm::scale(model, scale);
			model *= glm::toMat4(rotation);
			model[3] = vec4(translation, 1.0f);

			return model;
		}

		/// <summary>
		/// Sets the position or translation of the mesh
		/// </summary>
		void SetTranslation(vec3 translation) { this->translation = translation; }

		/// <summary>
		/// Returns the position or translation of the mesh
		/// </summary>
		vec3 GetTranslation() { return translation; }

		/// <summary>
		/// Sets the rotation of the mesh about its center
		/// </summary>
		void SetRotation(fquat rotation) { this->rotation = rotation; }

		/// <summary>
		/// Returns the rotation of the mesh about its center
		/// </summary>
		fquat GetRotation() { return rotation; }

		/// <summary>
		/// Returns the size or scale of the mesh
		/// </summary>
		vec3 GetScale() { return scale; }

		/// <summary>
		/// Sets the size or scale of the mesh
		/// </summary>
		vec3 SetScale(vec3 scale) { this->scale = scale; }

	protected:
		VertexBuffer vBuf;
		IndexBuffer iBuf;

		vec3 translation;
		vec3 scale;
		fquat rotation;
	};
};