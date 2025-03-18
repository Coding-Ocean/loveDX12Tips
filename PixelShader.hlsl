#include<Header.hlsli>

float hit_sphere(const float3 center, float radius, float3 rayOrigin, float3 rayDirection)
{
    float3 oc = rayOrigin - center;
    float a = dot(rayDirection, rayDirection);
    float b = dot(oc, rayDirection);
    float c = dot(oc, oc) - radius * radius;
    float discriminant = b * b - a * c;
    if (discriminant < 0)
    {
        return -1.0;
    }
    else
    {
        return (-b - sqrt(discriminant)) / a;
    }
}

float4 ray_color(float3 rayOrigin, float3 rayDirection)
{
    float3 light = normalize(float3(sin(Time), 0.5f, cos(Time)));

    //sphere
    float3 center = float3(0, 0, -1);
    float radius = 0.5;
    float t = hit_sphere(center, radius, rayOrigin, rayDirection);
    if (t > 0.0)
    {
        float3 N = normalize(rayOrigin + rayDirection * t - center);
        float3 bright = dot(light, N);
        return float4(bright, 1);
    }

    //ground sphere
    center = float3(0, -100.2, -1);
    radius = 100;
    t = hit_sphere(center, radius, rayOrigin, rayDirection);
    if (t > 0.0)
    {
        float3 N = normalize(rayOrigin + rayDirection * t - center);
        float3 bright = dot(light, N);
        return float4(bright, 1);
    }
    
    //background
    float3 unit_direction = normalize(rayDirection);
    t = 0.5 * (unit_direction.y + 1.0);
    float3 color = (1.0 - t) * float3(1.0, 1.0, 1.0) + t * float3(0.5, 0.7, 1.0);
    return float4(color, 1);
}

float4 main(float4 i_pos : SV_POSITION, float2 i_uv : TEXCOORD) : SV_TARGET
{
    float3 rayOrigin = float3(0, 0, 0);
    float3 rayDirection = float3(i_uv, -1) - rayOrigin;
    return ray_color(rayOrigin, rayDirection);
}
