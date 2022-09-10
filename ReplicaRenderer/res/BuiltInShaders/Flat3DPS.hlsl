float4 main(float4 pos : SV_Position) : SV_Target
{
	const float2 zPos = pos.xy * pos.z,
		grad = float2(ddx(zPos.x), ddy(zPos.y));

	return float4(grad, 0.5f, 1.0f);
}