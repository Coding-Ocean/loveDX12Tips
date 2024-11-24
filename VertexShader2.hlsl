#include<Header.hlsli>
void main(
    in float4 i_pos : POSITION,
    in float2 i_uv : TEXCOORD,
    out float4 o_sv_pos : SV_POSITION,
    out float2 o_uv : TEXCOORD
)
{
    o_sv_pos = i_pos;
    //o_sv_pos = float4(i_pos.x + Offset.x, i_pos.y + Offset.y, i_pos.z, 1);
    o_uv = i_uv;
}
