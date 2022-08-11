struct VertIn
{
	float2 pos : Position;
	float3 color : Color;
};

struct VertOut
{
	float4 pos : SV_Position;
	float3 color : Color;
};

cbuffer cBuf
{
	float4 tint;
};

VertOut main(VertIn i)
{
	VertOut o;
	o.pos = float4(i.pos.xy, 0.0f, 1.0f);
	o.color = i.color * tint.rgb;

	return o;
}