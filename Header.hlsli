cbuffer b0 : register(b0)
{
    float4 LightPos;
    float4 EyePos;
    matrix WorldViewProj;
    matrix World;
}
cbuffer b1 : register(b1)
{
    float4 Ambient;
    float4 Diffuse;
    float4 Specular;
}
Texture2D<float4> Texture : register(t0); //テクスチャ0番
SamplerState Sampler : register(s0); //サンプラ0番

