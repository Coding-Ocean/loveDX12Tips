#include<Header.hlsli>
matrix FetchBoneWorld(uint boneIdx)
{
    return BoneWorld[boneIdx];
}
void main(
    in float4 i_pos : POSITION,
    in float4 i_normal : NORMAL,
    in float2 i_uv : TEXCOORD,
	in float2 i_bones : BONE_INDEX,
    in float2 i_weights : BONE_WEIGHT,
    out float4 o_pos : SV_POSITION,
    out float4 o_diffuse : COLOR,
    out float2 o_uv : TEXCOORD)
{
    float4 pos = { 0,0,0,1 };
    float3 norm = { 0,0,0 };
    
	//ボーン0
    uint boneIdx = i_bones.x;
    float fWeight = i_weights.x;
    matrix m = FetchBoneWorld(boneIdx);
    pos  += fWeight * mul(m, i_pos);
    norm += fWeight * mul((float3x3)m, i_normal.xyz);
	//ボーン1
    boneIdx = i_bones.y;
    fWeight = i_weights.y;
    m = FetchBoneWorld(boneIdx);
    pos  += fWeight * mul(m, i_pos);
    norm += fWeight * mul((float3x3) m, i_normal.xyz);
    
    //
    o_pos = mul(WorldViewProj, pos);
    
    norm = normalize(mul((float3x3)World, norm));
    float brightness = max(0, dot(norm, LightPos.xyz));
    o_diffuse = Ambient + Diffuse * brightness;
    o_diffuse.a = Diffuse.a;
    
    o_uv = i_uv;
}
