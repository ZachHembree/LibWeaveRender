#include "pch.hpp"
#include "ReplicaD3D11.hpp"

namespace Replica::D3D11
{
	Mesh::Mesh(
		Device& device, 
		const IDynamicArray<USHORT>& indices, 
		const void* vertices, 
		size_t vCount, 
		size_t vStride
	) :
		vBuf(device, vertices, vCount, vStride),
		iBuf(device, indices),
		translation(0),
		rotation(1, 0, 0, 0),
		scale(1)
	{ }

	Mesh::Mesh() : translation(0), rotation(0, 0, 0, 0), scale(0)
	{ }

	/// <summary>
	/// Updates any resources needed prior to drawing
	/// </summary>
	void Mesh::Setup(Context& ctx)
	{
		vBuf.Bind(ctx);
		iBuf.Bind(ctx);
	}

	/// <summary>
	/// Draws the object
	/// </summary>
	void Mesh::Draw(Context& ctx, Material& mat)
	{
		ctx.Draw(*this, mat);
	}

	/// <summary>
	/// Returns the number of indices in the index buffer
	/// </summary>
	UINT Mesh::GetIndexCount() const { return iBuf.GetLength(); }

	/// <summary>
	/// Retrieves the model matrix representing the translation, rotation and
	/// scale of the object
	/// </summary>
	mat4 Mesh::GetModelMatrix()
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
	void Mesh::SetTranslation(vec3 translation) { this->translation = translation; }

	/// <summary>
	/// Returns the position or translation of the mesh
	/// </summary>
	vec3 Mesh::GetTranslation() { return translation; }

	/// <summary>
	/// Sets the rotation of the mesh about its center
	/// </summary>
	void Mesh::SetRotation(fquat rotation) { this->rotation = rotation; }

	/// <summary>
	/// Returns the rotation of the mesh about its center
	/// </summary>
	fquat Mesh::GetRotation() { return rotation; }

	/// <summary>
	/// Returns the size or scale of the mesh
	/// </summary>
	vec3 Mesh::GetScale() { return scale; }

	/// <summary>
	/// Sets the size or scale of the mesh
	/// </summary>
	void Mesh::SetScale(vec3 scale) { this->scale = scale; }
}