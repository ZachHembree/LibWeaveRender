cbuffer _
{
	float4 DstTexelSize;
};

RWTexture2D<float4> dstTex;

[numthreads(1, 1, 1)]
void main( uint3 id : SV_DispatchThreadID )
{
	dstTex[id.xy] = float4((id.xy + 0.5f) * DstTexelSize.xy, 0, 1);
}