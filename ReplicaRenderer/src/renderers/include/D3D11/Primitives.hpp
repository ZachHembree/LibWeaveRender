#pragma once
#pragma warning(disable: 4267)

#include "D3D11/Mesh.hpp"

namespace Replica::D3D11::Primitives
{
	/// <summary>
	/// Generates a cone mesh with the given number of subdivisions
	/// </summary>
	template<typename Vert_T>
	MeshDef<Vert_T> GenerateCone(int subdivisions)
	{
		constexpr float tau = 2.0f * glm::pi<float>();
		const float inc = tau / subdivisions;
		const int vertCount = (subdivisions + 2);
		float rad = 0.0f;

		UniqueArray<Vert_T> vertices(vertCount);
		UniqueArray<USHORT> indices(3 * 2 * vertCount + 3);

		for (int i = 0; i < (int)vertices.GetLength() - 2; i++)
		{
			vertices[i].pos = 0.5f * vec3(cos(rad), sin(rad), -1.0f);
			rad += inc;
		}

		const int p1 = vertices.GetLength() - 1,
			p0 = p1 - 1,
			vmax = p0;

		vertices[p0].pos = vec3(0, 0, -0.5f);
		vertices[p1].pos = vec3(0, 0, +0.5);

		for (int i = 0; i < (int)vertices.GetLength(); i++)
		{
			int start = 3 * i;
			indices[start + 0] = p0;
			indices[start + 2] = i % vmax;
			indices[start + 1] = (i + 1) % vmax;
		}

		int h2 = indices.GetLength() / 2 + 2;

		for (int i = 0; i < (int)vertices.GetLength(); i++)
		{
			int start = h2 + 3 * i;
			indices[start + 2] = p1;
			indices[start + 0] = i % vmax;
			indices[start + 1] = (i + 1) % vmax;
		}

		return MeshDef(std::move(vertices), std::move(indices));
	}

	/// <summary>
	/// Generates a cylinder mesh with the given number of subdivisions
	/// </summary>
	template<typename Vert_T>
	MeshDef<Vert_T> GenerateCylinder(int subdivisions)
	{
		constexpr float tau = 2.0f * glm::pi<float>();
		const float inc = tau / subdivisions;
		const int vertCount = 2 * (subdivisions + 1),
			vMid = (vertCount / 2 - 1);
		float rad = 0.0f;

		UniqueArray<Vert_T> vertices(vertCount);
		UniqueArray<USHORT> indices((3 * 4 * vertCount));

		for (int i = 0; i < vMid; i++)
		{
			vec2 flatPos(0.5f * cos(rad), 0.5f * sin(rad));
			vertices[i].pos = vec3(flatPos, -0.5f);
			vertices[vMid + i].pos = vec3(flatPos, 0.5f);
			rad += inc;
		}

		const int p1 = vertCount - 1,
			p0 = p1 - 1;

		vertices[p0].pos = vec3(0, 0, -0.5f);
		vertices[p1].pos = vec3(0, 0, +0.5);

		// Top and bottom
		for (int i = 0; i < vMid; i++)
		{
			int tStart = 3 * i;
			indices[tStart + 0] = p0;
			indices[tStart + 2] = i;
			indices[tStart + 1] = (i + 1) % vMid;
		}

		for (int i = 0; i < vMid; i++)
		{
			int tStart = 3 * vMid + 3 * i;
			indices[tStart + 0] = p1;
			indices[tStart + 1] = vMid + i;
			indices[tStart + 2] = vMid + (i + 1) % vMid;
		}

		const int vMax = 2 * vMid;

		// Sides
		for (int i = 0; i < vMid; i++)
		{
			int tStart = 6 * vMid + 6 * i;
			indices[tStart + 0] = i;
			indices[tStart + 1] = (i + 1) % vMid;
			indices[tStart + 2] = vMid + i;

			indices[tStart + 3] = vMid + (i + 1) % vMid;
			indices[tStart + 4] = vMid + i;
			indices[tStart + 5] = (i + 1) % vMid;
		}

		return MeshDef(std::move(vertices), std::move(indices));
	}

	/// <summary>
	/// Generates a subdivided UV sphere mesh
	/// </summary>
	template<typename Vert_T>
	MeshDef<Vert_T> GenerateSphere(ivec2 subdiv)
	{
		constexpr float pi = glm::pi<float>();
		const ivec2 vDim = subdiv + 1;
		const vec2 inc = pi / vec2(subdiv);
		UniqueArray<Vert_T> vertices(vDim.x * vDim.y);
		UniqueArray<USHORT> indices(3 * 2 * subdiv.x * (subdiv.y - 1));

		for (int i = 0; i < vDim.x; i++)
		{
			const float cos2I = cos(2 * i * inc.x),
				sin2I = sin(2 * i * inc.x);

			for (int j = 0; j < vDim.y; j++)
			{
				const float sinJ = sin(j * inc.y),
					cosJ = cos(j * inc.y);

				vertices[i * vDim.y + j].pos = 0.5f * vec3(sinJ * cos2I, sinJ * sin2I, cosJ);
			}
		}

		for (int i = 0; i < subdiv.x; i++)
		{
			int j = 0;
			int tStart = 6 * i * (subdiv.y - 1);
			ivec2 ll(i, j),
				ur(ll + 1);

			// Half of the triangles are degenerate at the poles
			indices[tStart + 1] = ll.x * vDim.y + ur.y;
			indices[tStart + 2] = ur.x * vDim.y + ur.y;
			indices[tStart + 3] = ur.x * vDim.y + ll.y;
			tStart += 3;
			j++;

			for (; j < subdiv.y - 1; j++)
			{
				ll = ivec2(i, j);
				ur = ivec2(ll + 1);

				indices[tStart] = ll.x * vDim.y + ll.y;
				indices[tStart + 1] = ll.x * vDim.y + ur.y;
				indices[tStart + 2] = ur.x * vDim.y + ll.y;

				indices[tStart + 3] = ll.x * vDim.y + ur.y;
				indices[tStart + 4] = ur.x * vDim.y + ur.y;
				indices[tStart + 5] = ur.x * vDim.y + ll.y;
				tStart += 6;
			}

			ll = ivec2(i, j);
			ur = ivec2(ll + 1);

			indices[tStart] = ll.x * vDim.y + ll.y;
			indices[tStart + 1] = ll.x * vDim.y + ur.y;
			indices[tStart + 2] = ur.x * vDim.y + ll.y;
		}

		return MeshDef(std::move(vertices), std::move(indices));
	}

	/// <summary>
	/// Generates subdivided plane
	/// </summary>
	template<typename Vert_T>
	MeshDef<Vert_T> GeneratePlane(ivec2 subdiv = ivec2(0), float scale = 1.0f)
	{
		subdiv++;

		const vec2 incr(scale / vec2(subdiv));
		const ivec2 dim = subdiv + 1;
		UniqueArray<Vert_T> vertices(dim.x * dim.y);
		UniqueArray<USHORT> indices(6 * subdiv.x * subdiv.y);
		scale *= 0.5f;

		for (int i = 0; i < dim.x; i++)
		{
			for (int j = 0; j < dim.y; j++)
			{
				vertices[i * dim.y + j].pos = vec3(i * incr.x, j * incr.y, scale) - scale;
			}
		}

		for (int i = 0; i < subdiv.x; i++)
		{
			for (int j = 0; j < subdiv.y; j++)
			{
				const int vStart = i * dim.y + j,
					tStart = 6 * (i * subdiv.y + j);

				indices[tStart] = vStart;
				indices[tStart + 1] = vStart + 1;
				indices[tStart + 2] = vStart + dim.y;

				indices[tStart + 3] = vStart + 1;
				indices[tStart + 4] = vStart + dim.y + 1;
				indices[tStart + 5] = vStart + dim.y;
			}
		}

		return MeshDef(std::move(vertices), std::move(indices));
	}

	/// <summary>
	/// Generates subdivided cube
	/// </summary>
	template<typename Vert_T>
	MeshDef<Vert_T> GenerateCube(ivec2 subdiv = ivec2(0))
	{
		subdiv++;

		const ivec2 dim = subdiv + 1;
		const vec2 incr = 1.0f / vec2(subdiv);
		const int faceSize = dim.x * dim.y;
		UniqueArray<Vert_T> vertices(6 * faceSize);
		UniqueArray<USHORT> indices(6 * vertices.GetLength());

		for (int i = 0; i < dim.x; i++)
		{
			for (int j = 0; j < dim.y; j++)
			{
				int v = i * dim.y + j;
				vec3 a = vec3(vec2(i, j) * incr, 0) - 0.5f,
					b = vec3(a.x, a.y, 0.5);
					
				vertices[v].pos = a;
				vertices[v + faceSize].pos = vec3(a.z, a.x, a.y);
				vertices[v + 2 * faceSize].pos = vec3(a.y, a.z, a.x);

				vertices[v + 3 * faceSize].pos = b;
				vertices[v + 4 * faceSize].pos = vec3(b.z, b.x, b.y);
				vertices[v + 5 * faceSize].pos = vec3(b.y, b.z, b.x);
			}
		}

		// Negative faces
		for (int i = 0; i < 3; i++)
		{
			for (int j = 0; j < subdiv.x; j++)
			{
				for (int k = 0; k < subdiv.y; k++)
				{
					const int vStart = i * faceSize + j * dim.y + k,
						tStart = 6 * (i * faceSize + j * subdiv.y + k);

					indices[tStart] = vStart;
					indices[tStart + 1] = vStart + 1;
					indices[tStart + 2] = vStart + dim.y;

					indices[tStart + 3] = vStart + 1;
					indices[tStart + 4] = vStart + dim.y + 1;
					indices[tStart + 5] = vStart + dim.y;
				}
			}
		}

		// Positive faces
		for (int i = 3; i < 6; i++)
		{
			for (int j = 0; j < subdiv.x; j++)
			{
				for (int k = 0; k < subdiv.y; k++)
				{
					const int vStart = i * faceSize + j * dim.y + k,
						tStart = 6 * (i * faceSize + j * subdiv.y + k);

					indices[tStart] = vStart;
					indices[tStart + 1] = vStart + dim.y;
					indices[tStart + 2] = vStart + 1; 

					indices[tStart + 3] = vStart + 1;
					indices[tStart + 4] = vStart + dim.y;
					indices[tStart + 5] = vStart + dim.y + 1;
				}
			}
		}

		return MeshDef(std::move(vertices), std::move(indices));
	}
}