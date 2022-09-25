cbuffer _
{
	int2 SrcOffset;
	int2 DstOffset;
};

Texture2D<float4> SrcTex;
RWTexture2D<float4> DstTex;

[numthreads(1, 1, 1)]
void main(uint3 id : SV_DispatchThreadID)
{
	DstTex[id.xy + DstOffset] = SrcTex[id.xy + SrcOffset];
}