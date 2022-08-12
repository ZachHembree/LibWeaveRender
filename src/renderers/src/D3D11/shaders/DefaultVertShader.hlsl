struct VertIn
{
	float3 pos : Position;
	float3 color : Color;
};

struct VertOut
{
	float4 pos : SV_Position;
	float3 color : Color;
};

cbuffer _
{
	float4 tint;
	float4x4 mvp;
};

VertOut main(VertIn i)
{
	VertOut o;

	o.pos = mul(float4(i.pos, 1.0f), mvp);
	o.color = i.color;

	return o;
}