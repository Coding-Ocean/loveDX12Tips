#include<Header.hlsli>

#define MAX_STEPS 100
#define MAX_DIST 200.0
#define SURF_DIST 0.01

float GetDist(float3 p)
{
    //sphere
    float4 s = float4(0, 1, 5, 1);//x, y, z, radius
    float sphereDist = length(p - s.xyz) - s.w;
    
    //plane
    float planeDist = p.y;
    
    float dist = min(sphereDist, planeDist);
    return dist;
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
        GetDist(p - e.yyx)
    );
    
    return normalize(n);
}

float Lighting(float3 p)
{
    float3 lightPos = float3(0, 6, 3);
    lightPos.xz += float2(sin(Time), cos(Time)) * 2;
    
    float3 l = normalize(lightPos - p);
    float3 n = GetNormal(p);
    
    float diffuse = clamp(dot(n, l), 0., 1.);
    float d = RayMarch(p + n * SURF_DIST * 2., l);
    if (d < length(lightPos - p))
        diffuse *= .1;//‰e‚È‚Ì‚ÅˆÃ‚­‚·‚é
    
    return diffuse;
}

float4 main(float4 i_pos : SV_POSITION, float2 i_uv : TEXCOORD) : SV_TARGET
{
    float3 ro = float3(0, 1, 0);//ray origin
    float3 rd = normalize(float3(i_uv, 1));//ray direction

    float d = RayMarch(ro, rd);
    
    float3 col = 0;
    //col = 1 - d / 10; //‚„‚Ì’lŽ‹Šo‰»
    //return float4(col, 1);
    
    float3 p = ro + rd * d;
    float diffuse = Lighting(p);
    col = pow(diffuse, 0.4545); //gamma correction
    return float4(col, 1);
}
