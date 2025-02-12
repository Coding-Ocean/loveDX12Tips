#include<Header.hlsli>
float4 main(
    float4 i_pos : SV_POSITION,
    float4 i_diffuse : COLOR,
    float2 i_uv : TEXCOORD
    ) : SV_TARGET
{
    //return i_diffuse;
    return float4(Texture.Sample(Sampler, i_uv)) * i_diffuse;
}
