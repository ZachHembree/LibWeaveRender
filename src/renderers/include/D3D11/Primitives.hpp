#pragma once
#include "D3D11/Mesh.hpp"

namespace Replica::D3D11::Primitives
{
	template<typename Vert_t>
	static Mesh GenerateCone(Device& dev, int subdivisions)
	{
		constexpr float tau = 2.0f * glm::pi<float>();
		float inc = tau / subdivisions,
			rad = 0.0f;
		int vertCount = (subdivisions + 2);

		UniqueArray<Vert_t> vertices(vertCount);
		UniqueArray<USHORT> indices(3 * 2 * vertCount + 3);

		for (int i = 0; i < vertices.GetLength() - 2; i++)
		{
			vertices[i].pos = 0.5f * vec3(cos(rad), sin(rad), -1.0f);
			rad += inc;
		}

		int p1 = vertices.GetLength() - 1,
			p0 = p1 - 1,
			vmax = p0;

		vertices[p0].pos = vec3(0, 0, -0.5f);
		vertices[p1].pos = vec3(0, 0, +0.5);

		for (int i = 0; i < vertices.GetLength(); i++)
		{
			int start = 3 * i;
			indices[start + 0] = p0;
			indices[start + 2] = i % vmax;
			indices[start + 1] = (i + 1) % vmax;
		}

		int h2 = indices.GetLength() / 2 + 2;

		for (int i = 0; i < vertices.GetLength(); i++)
		{
			int start = h2 + 3 * i;
			indices[start + 2] = p1;
			indices[start + 0] = i % vmax;
			indices[start + 1] = (i + 1) % vmax;
		}

		return Mesh(dev, vertices, indices);
	}

	template<typename Vert_t>
	static Mesh GenerateCylinder(Device& dev, int subdivisions)
	{
		constexpr float tau = 2.0f * glm::pi<float>();
		float inc = tau / subdivisions,
			rad = 0.0f;
		int vertCount = 2 * (subdivisions + 1),
			vMid = (vertCount / 2 - 1);

		UniqueArray<Vert_t> vertices(vertCount);
		UniqueArray<USHORT> indices((3 * 4 * vertCount));

		for (int i = 0; i < vMid; i++)
		{
			vec2 flatPos(0.5f * cos(rad), 0.5f * sin(rad));
			vertices[i].pos = vec3(flatPos, -0.5f);
			vertices[vMid + i].pos = vec3(flatPos, 0.5f);
			rad += inc;
		}

		int p1 = vertCount - 1,
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

		int vMax = 2 * vMid;

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

		return Mesh(dev, vertices, indices);
	}
}