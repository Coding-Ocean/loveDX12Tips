#include<Header.hlsli>
void main(
    in float4 i_pos     : POSITION,
    in float4 i_normal  : NORMAL,
    in float2 i_uv      : TEXCOORD,
    out float4 o_sv_pos : SV_POSITION,
    out float4 o_pos    : TEXCOORD0,
    out float4 o_normal : TEXCOORD1,
    out float2 o_uv     : TEXCOORD2
)
{
    o_sv_pos = mul(WorldViewProj, i_pos);
    
    o_pos = mul(World, i_pos);
    
    i_normal.w = 0;
    o_normal = normalize(mul(World, i_normal));
    
    o_uv = i_uv;
}
