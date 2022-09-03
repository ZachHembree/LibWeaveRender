struct VertIn
{
	float3 pos : Position;
};

struct VertOut
{
	float4 pos : SV_Position;
	float2 uv : TexCoord;
};

cbuffer _
{
	float4x4 mvp;
};

VertOut main(VertIn i)
{
	VertOut o;

	o.pos = mul(float4(i.pos, 1.0f), mvp);
	o.uv = i.pos.xy;

	return o;
}