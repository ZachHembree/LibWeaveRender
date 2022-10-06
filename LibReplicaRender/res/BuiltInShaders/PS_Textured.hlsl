struct VertOut
{
	float4 pos : SV_Position;
	float2 uv : TexCoord0;
};

cbuffer _
{
	float2 Scale;
	float2 Offset;
};

Texture2D tex;
SamplerState samp;

float4 main(VertOut i) : SV_Target
{
	return tex.Sample(samp, Scale * i.uv + Offset);
}