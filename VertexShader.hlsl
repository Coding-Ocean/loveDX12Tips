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
    i_normal.w = 0;
    float4 normal = { 0, 0, 0, 0 };
    
	//ボーン0
    uint boneIdx = i_bones.x;
    float fWeight = i_weights.x;
    matrix m = FetchBoneWorld(boneIdx);
    o_pos  = fWeight * mul(m, i_pos);
    normal = fWeight * mul(m, i_normal);
	//ボーン1
    boneIdx = i_bones.y;
    fWeight = i_weights.y;
    m = FetchBoneWorld(boneIdx);
    o_pos  += fWeight * mul(m, i_pos);
    normal += fWeight * mul(m, i_normal);
    
    o_pos = mul(ViewProj, o_pos);
    
    float brightness = max(0, dot(normal, LightPos));
    o_diffuse = Ambient + Diffuse * brightness;
    o_diffuse.a = Diffuse.a;
    
    o_uv = i_uv;
}
