#include<Header.hlsli>
void main(
    in float4 i_pos : POSITION,
    in float4 i_normal : NORMAL,
    in float2 i_uv : TEXCOORD,
	in float2 i_boneIdxs : BONE_INDEX,
    in float2 i_weights : BONE_WEIGHT,
    out float4 o_pos : SV_POSITION,
    out float4 o_diffuse : COLOR,
    out float2 o_uv : TEXCOORD)
{
    i_normal.w = 0;
    float4 normal = { 0, 0, 0, 0 };
    
	//�{�[��0
    float weight = i_weights.x;
    uint boneIdx = i_boneIdxs.x;
    matrix world = BoneWorld[boneIdx];
    o_pos  = weight * mul(world, i_pos);
    normal = weight * mul(world, i_normal);
	//�{�[��1
    weight = i_weights.y;
    boneIdx = i_boneIdxs.y;
    world = BoneWorld[boneIdx];
    o_pos  += weight * mul(world, i_pos);
    normal += weight * mul(world, i_normal);
    
    //�ʒu
    o_pos = mul(ViewProj, o_pos);
    //�f�B�t���[�Y�F
    float brightness = max(0, dot(normal, LightPos));
    o_diffuse = Ambient + Diffuse * brightness;
    o_diffuse.a = Diffuse.a;
    //�e�N�X�`�����W
    o_uv = i_uv;
}
