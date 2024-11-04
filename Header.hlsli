cbuffer b0 : register(b0)
{
    float4 LightPos;
    matrix ViewProj;
    matrix BoneWorlds[2];
}
cbuffer b1 : register(b1)
{
    float4 Ambient;
    float4 Diffuse;
}
Texture2D<float4> Texture : register(t0); //�e�N�X�`��0��
SamplerState Sampler : register(s0); //�T���v��0��

