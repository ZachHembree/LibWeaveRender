struct VertOut
{
	float4 pos : SV_Position;
	float2 uv : TexCoord;
};

Texture2D tex;
SamplerState samp;

float4 main(VertOut i) : SV_Target
{
	//return float4(i.uv, 0, 0);
	return tex.Sample(samp, i.uv);
}