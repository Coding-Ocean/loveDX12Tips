cbuffer b0 : register(b0)
{
    matrix ViewProj;
    float4 LightPos;
    matrix BoneWorld[2];
}
cbuffer b1 : register(b1)
{
    float4 Ambient;
    float4 Diffuse;
}
Texture2D<float4> Texture : register(t0); //テクスチャ0番
SamplerState Sampler : register(s0); //サンプラ0番

