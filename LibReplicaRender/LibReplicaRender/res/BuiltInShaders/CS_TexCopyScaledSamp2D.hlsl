cbuffer _
{
	float4 DstTexelSize;
	float2 Scale;
	float2 Offset;
};

Texture2D<float4> SrcTex;
RWTexture2D<float4> DstTex;
SamplerState Samp;

[numthreads(1, 1, 1)]
void main(uint3 id : SV_DispatchThreadID)
{
	const float2 uv = Scale * (id.xy + 0.5f) * DstTexelSize.xy;
	DstTex[id.xy] = SrcTex.SampleLevel(Samp, Offset + uv, 0);
}