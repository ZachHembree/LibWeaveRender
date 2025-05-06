#include "pch.hpp"
#include "D3D11/InternalD3D11.hpp"
#include "D3D11/CtxBase.hpp"
#include "D3D11/Mesh.hpp"
#include "D3DUtils.hpp"

namespace Weave::D3D11
{
	const PrimTopology PrimTopology::UNDEFINED = D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED;
	const PrimTopology PrimTopology::POINTLIST = D3D11_PRIMITIVE_TOPOLOGY_POINTLIST;
	const PrimTopology PrimTopology::LINELIST = D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
	const PrimTopology PrimTopology::LINESTRIP = D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP;
	const PrimTopology PrimTopology::TRIANGLELIST = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	const PrimTopology PrimTopology::TRIANGLESTRIP = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
	const PrimTopology PrimTopology::LINELIST_ADJ = D3D11_PRIMITIVE_TOPOLOGY_LINELIST_ADJ;
	const PrimTopology PrimTopology::LINESTRIP_ADJ = D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP_ADJ;
	const PrimTopology PrimTopology::TRIANGLELIST_ADJ = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST_ADJ;
	const PrimTopology PrimTopology::TRIANGLESTRIP_ADJ = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP_ADJ;
	const PrimTopology PrimTopology::CP1_PATCHLIST = D3D11_PRIMITIVE_TOPOLOGY_1_CONTROL_POINT_PATCHLIST;
	const PrimTopology PrimTopology::CP2_PATCHLIST = D3D11_PRIMITIVE_TOPOLOGY_2_CONTROL_POINT_PATCHLIST;
	const PrimTopology PrimTopology::CP3_PATCHLIST = D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST;
	const PrimTopology PrimTopology::CP4_PATCHLIST = D3D11_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST;
	const PrimTopology PrimTopology::CP5_PATCHLIST = D3D11_PRIMITIVE_TOPOLOGY_5_CONTROL_POINT_PATCHLIST;
	const PrimTopology PrimTopology::CP6_PATCHLIST = D3D11_PRIMITIVE_TOPOLOGY_6_CONTROL_POINT_PATCHLIST;
	const PrimTopology PrimTopology::CP7_PATCHLIST = D3D11_PRIMITIVE_TOPOLOGY_7_CONTROL_POINT_PATCHLIST;
	const PrimTopology PrimTopology::CP8_PATCHLIST = D3D11_PRIMITIVE_TOPOLOGY_8_CONTROL_POINT_PATCHLIST;
	const PrimTopology PrimTopology::CP9_PATCHLIST = D3D11_PRIMITIVE_TOPOLOGY_9_CONTROL_POINT_PATCHLIST;
	const PrimTopology PrimTopology::CP10_PATCHLIST = D3D11_PRIMITIVE_TOPOLOGY_10_CONTROL_POINT_PATCHLIST;
	const PrimTopology PrimTopology::CP11_PATCHLIST = D3D11_PRIMITIVE_TOPOLOGY_11_CONTROL_POINT_PATCHLIST;
	const PrimTopology PrimTopology::CP12_PATCHLIST = D3D11_PRIMITIVE_TOPOLOGY_12_CONTROL_POINT_PATCHLIST;
	const PrimTopology PrimTopology::CP13_PATCHLIST = D3D11_PRIMITIVE_TOPOLOGY_13_CONTROL_POINT_PATCHLIST;
	const PrimTopology PrimTopology::CP14_PATCHLIST = D3D11_PRIMITIVE_TOPOLOGY_14_CONTROL_POINT_PATCHLIST;
	const PrimTopology PrimTopology::CP15_PATCHLIST = D3D11_PRIMITIVE_TOPOLOGY_15_CONTROL_POINT_PATCHLIST;
	const PrimTopology PrimTopology::CP16_PATCHLIST = D3D11_PRIMITIVE_TOPOLOGY_16_CONTROL_POINT_PATCHLIST;
	const PrimTopology PrimTopology::CP17_PATCHLIST = D3D11_PRIMITIVE_TOPOLOGY_17_CONTROL_POINT_PATCHLIST;
	const PrimTopology PrimTopology::CP18_PATCHLIST = D3D11_PRIMITIVE_TOPOLOGY_18_CONTROL_POINT_PATCHLIST;
	const PrimTopology PrimTopology::CP19_PATCHLIST = D3D11_PRIMITIVE_TOPOLOGY_19_CONTROL_POINT_PATCHLIST;
	const PrimTopology PrimTopology::CP20_PATCHLIST = D3D11_PRIMITIVE_TOPOLOGY_20_CONTROL_POINT_PATCHLIST;
	const PrimTopology PrimTopology::CP21_PATCHLIST = D3D11_PRIMITIVE_TOPOLOGY_21_CONTROL_POINT_PATCHLIST;
	const PrimTopology PrimTopology::CP22_PATCHLIST = D3D11_PRIMITIVE_TOPOLOGY_22_CONTROL_POINT_PATCHLIST;
	const PrimTopology PrimTopology::CP23_PATCHLIST = D3D11_PRIMITIVE_TOPOLOGY_23_CONTROL_POINT_PATCHLIST;
	const PrimTopology PrimTopology::CP24_PATCHLIST = D3D11_PRIMITIVE_TOPOLOGY_24_CONTROL_POINT_PATCHLIST;
	const PrimTopology PrimTopology::CP25_PATCHLIST = D3D11_PRIMITIVE_TOPOLOGY_25_CONTROL_POINT_PATCHLIST;
	const PrimTopology PrimTopology::CP26_PATCHLIST = D3D11_PRIMITIVE_TOPOLOGY_26_CONTROL_POINT_PATCHLIST;
	const PrimTopology PrimTopology::CP27_PATCHLIST = D3D11_PRIMITIVE_TOPOLOGY_27_CONTROL_POINT_PATCHLIST;
	const PrimTopology PrimTopology::CP28_PATCHLIST = D3D11_PRIMITIVE_TOPOLOGY_28_CONTROL_POINT_PATCHLIST;
	const PrimTopology PrimTopology::CP29_PATCHLIST = D3D11_PRIMITIVE_TOPOLOGY_29_CONTROL_POINT_PATCHLIST;
	const PrimTopology PrimTopology::CP30_PATCHLIST = D3D11_PRIMITIVE_TOPOLOGY_30_CONTROL_POINT_PATCHLIST;
	const PrimTopology PrimTopology::CP31_PATCHLIST = D3D11_PRIMITIVE_TOPOLOGY_31_CONTROL_POINT_PATCHLIST;
	const PrimTopology PrimTopology::CP32_PATCHLIST = D3D11_PRIMITIVE_TOPOLOGY_32_CONTROL_POINT_PATCHLIST;

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