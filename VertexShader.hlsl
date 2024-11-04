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
    
	//ボーン0
    matrix world = BoneWorlds[(int)i_boneIdxs.x];
    o_pos = i_weights.x * mul(world, i_pos);
    normal = i_weights.x * mul(world, i_normal);
	//ボーン1
    world = BoneWorlds[(int) i_boneIdxs.y];
    o_pos += i_weights.y * mul(world, i_pos);
    normal += i_weights.y * mul(world, i_normal);
    
    //位置
    o_pos = mul(ViewProj, o_pos);
    //ディフューズ色
    float brightness = max(0, dot(normal, LightPos));
    o_diffuse = Ambient + Diffuse * brightness;
    o_diffuse.a = Diffuse.a;
    //テクスチャ座標
    o_uv = i_uv;
}
