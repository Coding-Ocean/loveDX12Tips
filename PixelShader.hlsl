#include<Header.hlsli>

#define MAX_STEPS 80
#define MAX_DIST 100.0
#define SURF_DIST 0.001
float2x2 rot2D(float t)
{
    return float2x2(cos(t), -sin(t), sin(t), cos(t));
}
float smin(float a, float b, float k)
{
    float h = max(k - abs(a - b), 0.) / k;
    return min(a, b) - h * h * h * k * (1. / 6.);
}
float GetDist(float3 p)
{
    //sphere
    float3 spherePos = float3(cos(Time*0.3)*1.2, 1, 0);
    float sphere = length(p - spherePos) - 1.;
    //float sphere2 = length(p - float3(-cos(Time*0.3)*1.2, 1, 0)) - 1.;
    
    //box
    float3 p_ = p;
    p_.y -= 1;
    p_.xz = mul(p_.xz, rot2D(Time*2));
    p_.xy = mul(p_.xy, rot2D(Time*2));
    float3 q = abs(p_) - .5;
    float box = length(max(q, 0.0)) + min(max(q.x, max(q.y, q.z)), 0.0);
    
    //plane
    float plane = p.y+1.;
    
    //return min(plane, smin(sphere, sphere2, 1.));
    return min(plane, smin(sphere, box, 1.));
}
float RayMarch(float3 ro, float3 rd)
{
    float dO = 0.;
    
    for (int i = 0; i < MAX_STEPS; i++)
    {
        float3 p = ro + rd * dO;
        float dS = GetDist(p);
        dO += dS;
        if (dO > MAX_DIST || dS < SURF_DIST)
            break;
    }
    
    return dO;
}

float3 GetNormal(float3 p)
{
    float d = GetDist(p);
    float2 e = float2(0.01, 0);
    
    float3 n = d - float3(
        GetDist(p - e.xyy),
        GetDist(p - e.yxy),
        GetDist(p - e.yyx));
    
    return normalize(n);
}

float Lighting(float3 p)
{
    float3 lightPos = float3(0, 3, -2);
    //lightPos.xz += float2(sin(Time), cos(Time)) * 2;
    
    float3 l = normalize(lightPos - p);
    float3 n = GetNormal(p);
    
    float diffuse = clamp(dot(n, l), 0., 1.);
    float d = RayMarch(p + n * SURF_DIST * 2., l);
    if (d < length(lightPos - p))
        diffuse *= .5;//‰e‚È‚Ì‚ÅˆÃ‚­‚·‚é
    
    return diffuse;
}

float4 main(float4 i_pos : SV_POSITION, float2 i_uv : TEXCOORD) : SV_TARGET
{
    float3 ro = float3(0, 1, -5);//ray origin
    float3 rd = normalize(float3(i_uv, 1.));//ray direction
    float3 col = 0;//final color

    float d = RayMarch(ro, rd);
    
    //col = 1 - d / 10; //‚„‚Ì’lŽ‹Šo‰»
    //return float4(col, 1);
    
    float3 p = ro + rd * d;
    float diffuse = Lighting(p);
    col = pow(diffuse, 1); //gamma correction
    return float4(col, 1);
}
