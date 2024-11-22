#include<Header.hlsli>
void main(
    float4 i_sv_pos : SV_POSITION,
    float4 i_pos    : TEXCOORD0,
    float4 i_normal : TEXCOORD1,
    float2 i_uv     : TEXCOORD2,
    out float4 o_col0:SV_TARGET0,
    out float4 o_col1:SV_TARGET1
    )
{
    float4 t_col = Texture.Sample(Sampler, i_uv);
    
    float3 lightDir = normalize(LightPos.xyz - i_pos.xyz);
    float4 d_col = Ambient + Diffuse * max(0, dot(lightDir, i_normal.xyz));
    d_col.w = Diffuse.w;
    
    float3 eyeDir = normalize(EyePos.xyz - i_pos.xyz);
    float3 halfDir = normalize(lightDir + eyeDir);
    float4 s_col = Specular * pow(max(0, dot(halfDir, i_normal.xyz)), Specular.w);
    s_col.w = 0;

    o_col0 = saturate(t_col * d_col + s_col);
    o_col1 = o_col0;
}
