struct PsIn
{
	float4 pos : SV_Position;
	float2 uv : TexCoord;
};

Texture2D tex;
SamplerState samp;

float4 main(PsIn i) : SV_Target
{
	return tex.Sample(samp, i.uv);
}