#include "pch.hpp"
#include "D3D11/InternalD3D11.hpp"
#include "D3D11/CtxBase.hpp"
#include "D3D11/Mesh.hpp"
#include "D3D11/D3D11Utils.hpp"

namespace Weave::D3D11
{
	Mesh::Mesh(
		Device& device,
		const IDynamicArray<ushort>& indices,
		const void* vertices,
		uint vCount,
		uint vStride,
		PrimTopology topology
	) :
		vBuf(device, vertices, vCount, vStride),
		iBuf(device, indices),
		translation(0),
		rotation(1, 0, 0, 0),
		scale(1),
		topology(topology)
	{ }

	Mesh::Mesh(
		Device& device,
		const IDynamicArray<uint>& indices,
		const void* vertices,
		uint vCount,
		uint vStride,
		PrimTopology topology
	) :
		vBuf(device, vertices, vCount, vStride),
		iBuf(device, indices),
		translation(0),
		rotation(1, 0, 0, 0),
		scale(1),
		topology(topology)
	{}

	Mesh::Mesh() : 
		translation(0), 
		rotation(0, 0, 0, 0), 
		scale(0), 
		topology(PrimTopology::UNDEFINED)
	{ }

	PrimTopology Mesh::GetTopology() const { return topology; }

	/// <summary>
	/// Updates any resources needed prior to drawing
	/// </summary>
	void Mesh::Setup(CtxBase& ctx)
	{
		ctx.SetPrimitiveTopology(topology);
		ctx.BindVertexBuffer(vBuf);

		if (iBuf.GetLength() > 0)
			ctx.BindIndexBuffer(iBuf);
	}

	/// <summary>
	/// Draws the object
	/// </summary>
	void Mesh::Draw(CtxBase& ctx, Material& mat)
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