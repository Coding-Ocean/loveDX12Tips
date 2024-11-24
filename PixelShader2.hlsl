#include<Header.hlsli>
float4 main(
	float4 i_pos : SV_POSITION,
    float2 i_uv : TEXCOORD
) : SV_TARGET
{
    return Texture.Sample(Sampler, i_uv) * float4(2, .0f, 2.f, 1);
}