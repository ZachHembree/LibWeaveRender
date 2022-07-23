struct PsIn
{
	float4 pos : SV_Position;
	float3 color : Color;
};

float4 main(PsIn i) : SV_Target
{
	return float4(i.color.rgb, 1.0f);
}