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

VertOut main(VertIn i)
{
	VertOut o;
	o.pos = float4(i.pos.xy, 0.0f, 1.0f);
	o.color = i.color;

	return o;
}