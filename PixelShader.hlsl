#include<Header.hlsli>

float hit_sphere(const float3 center, float radius, const float3 rayDirection)
{
    float3 oc = RayOrigin - center;
    float a = dot(rayDirection, rayDirection);
    float b = 2.0 * dot(oc, rayDirection);
    float c = dot(oc, oc) - radius * radius;
    float discriminant = b * b - 4 * a * c;
    if (discriminant < 0)
    {
        return -1.0;
    }
    else
    {
        return (-b - sqrt(discriminant)) / (2.0 * a);
    }
}

float4 ray_color(float3 rayDirection)
{
    float3 center = float3(0, 0, -1);
    float t = hit_sphere(center, 0.5, rayDirection);
    if (t > 0.0)
    {
        float3 N = normalize(rayDirection*t - center);
        return float4(0.5 * float3(N.x + 1, N.y + 1, N.z + 1), 1);
    }
    //background
    float3 unit_direction = normalize(rayDirection);
    t = 0.5 * (unit_direction.y + 1.0);
    float3 color = (1.0 - t) * float3(0.5, 0.7, 1.0) + t * float3(1.0, 1.0, 1.0);
    return float4(color, 1);
}

float4 main(float4 i_pos : SV_POSITION, float2 i_uv : TEXCOORD) : SV_TARGET
{
    float3 rayDirection = float3(i_uv, -1) - RayOrigin;
    return ray_color(rayDirection);
}
