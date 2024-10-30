cbuffer b0 : register(b0)
{
    matrix WorldViewProj;
    matrix World;
    float4 LightDir;//原点からライトへの方向
}
cbuffer b1 : register(b1)
{
    float4 Ambient;
    float4 Diffuse;
}
Texture2D<float4> Texture : register(t0); //テクスチャ0番
SamplerState Sampler : register(s0); //サンプラ0番

