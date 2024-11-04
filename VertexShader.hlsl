#include<Header.hlsli>
void main(
    in float4 i_pos : POSITION,
    in float4 i_normal : NORMAL,
    in float2 i_uv : TEXCOORD,
    out float4 o_pos : SV_POSITION,
    out float4 o_diffuse : COLOR,
    out float2 o_uv : TEXCOORD)
{
    o_pos = mul(ViewProj, mul(World, i_pos));
    
    i_normal.w = 0;
    float4 normal = normalize(mul(World, i_normal));
    float brightness = max(0, dot(normal, LightPos));
    o_diffuse = Ambient + Diffuse * brightness;
    o_diffuse.a = Diffuse.a;
    
    o_uv = i_uv;
}
