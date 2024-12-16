#include<Header.hlsli>
float4 main(
    in float4 i_pos : SV_POSITION,
    in float2 i_uv : TEXCOORD
) : SV_TARGET
{
    return float4(Texture.Sample(Sampler, i_uv)) * Diffuse;
}
