#include<Header.hlsli>
float4 main(
    float4 i_sv_pos : SV_POSITION,
    float4 i_pos    : TEXCOORD0,
    float4 i_normal : TEXCOORD1,
    float2 i_uv     : TEXCOORD2
    ) : SV_TARGET
{
    float4 t_col = Texture.Sample(Sampler, i_uv);
    
    float3 lightDir = normalize(LightPos.xyz - i_pos.xyz);
    float4 d_col = Ambient + Diffuse * max(0, dot(lightDir, i_normal.xyz));
    d_col.w = Diffuse.w;
    
    //return d_col;
    return t_col * d_col;
}
