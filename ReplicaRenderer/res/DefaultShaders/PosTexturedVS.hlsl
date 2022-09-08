struct VertIn
{
	float2 pos : Position;
};

struct VertOut
{
	float4 pos : SV_Position;
	float2 uv : TexCoord;
};

VertOut main(VertIn i)
{
	VertOut o;
	o.pos = float4(i.pos, 0.0f, 1.0f);
	o.uv = float2(i.pos.x, -i.pos.y);
	o.uv = 0.5f * (o.uv + 1.0f);

	return o;
}