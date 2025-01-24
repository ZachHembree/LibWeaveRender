struct VertIn
{
	float2 pos : Position;
	float2 uv : TexCoord;
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
	o.uv = i.uv;

	return o;
}