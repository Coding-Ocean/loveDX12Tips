#include<Header.hlsli>
float4 main(
	float4 i_pos : SV_POSITION,
    float2 i_uv : TEXCOORD
) : SV_TARGET
{
    float3 color = float3(0.0, 0.0, 0.0);
    float weightTotal = 0.0;
    int blurUVCount = 60;
    for (int i = -blurUVCount; i <= blurUVCount; i+=3)
    {
        float weight = 1.0 - smoothstep(0.0, blurUVCount + 1, abs(i));
        color += Texture.Sample(Sampler, i_uv + BlurUVUnit.xy * i).xyz * weight;
        weightTotal += weight;
    }
    return float4(color / weightTotal, 1.0);
}