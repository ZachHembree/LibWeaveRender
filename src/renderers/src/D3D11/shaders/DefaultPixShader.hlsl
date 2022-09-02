struct PsIn
{
	float4 pos : SV_Position;
	float2 uv : TexCoord;
};

Texture2D tex;
SamplerState samp;

cbuffer _
{
	float4 DstTexelSize;
};

float4 main(PsIn i) : SV_Target
{
	//float2 screenPos = i.pos.xy * DstTexelSize.xy;
	return float4(i.pos.zzz, 1.0f);
}