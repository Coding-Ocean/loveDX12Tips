#include<Header.hlsli>

#define MAX_STEPS 80
#define MAX_DIST 90.0
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
//get distance from ray position to nearest surface
float GetDistFrom(float3 rp)
{
    //sphere
    float3 sp = float3(cos(Time * 0.3) * 2.5, 1, 0);//sphere position
    float sphere = length(rp - sp) - 1.;
    //sp.x *= -1;
    //float sphere2 = length(rp - sp) - 1.;
    
    //box
    float3 rp_ = rp;//copy
    rp_.y -= 1;
    rp_.xz = mul(rp_.xz, rot2D(Time*2));
    rp_.xy = mul(rp_.xy, rot2D(Time*2));
    float3 q = abs(rp_) - .6;
    float box = length(max(q, 0.0)) + min(max(q.x, max(q.y, q.z)), 0.0);
    
    //plane
    float plane = rp.y + 1.;
    
    //return min(plane, smin(sphere, sphere2, 1.));
    //return min(plane, box);
    return min(plane, smin(sphere, box, 1.));
}
float RayMarch(float3 ro, float3 rd)
{
    float t = 0.; //distance traveled
    for (int i = 0; i < MAX_STEPS; i++)
    {
        float3 rp = ro + rd * t;//ray position
        float d = GetDistFrom(rp);//get distance from ray position to nearest surface
        t += d;
        if (t > MAX_DIST || d < SURF_DIST)
            break;
    }
    return t;
}
float3 GetNormal(float3 rp)
{
    float d = GetDistFrom(rp);
    float2 sft = float2(0.001, 0); //shift value
    float3 n = d - float3(
        GetDistFrom(rp - sft.xyy), //rp-float3(0.01,0,0)
        GetDistFrom(rp - sft.yxy), //rp-float3(0,0.01,0)
        GetDistFrom(rp - sft.yyx) //rp-float3(0,0,0.01)
    );
    return normalize(n);
    
    //float2 sft = float2(0.001, 0);//shift value
    //float3 n = float3(
    //GetDistFrom(rp + sft.xyy) - GetDistFrom(rp - sft.xyy),
    //GetDistFrom(rp + sft.yxy) - GetDistFrom(rp - sft.yxy),
    //GetDistFrom(rp + sft.yyx) - GetDistFrom(rp - sft.yyx)
    //);
    //return normalize(n);
}
float Lighting(float3 rp)
{
    float3 lp = float3(0, 5, -3);//light position
    float3 lv = lp - rp;//light vector
    float len = length(lv);
    lv /= len; //normalize
    float3 nv = GetNormal(rp);
    float brightness = clamp(dot(nv, lv), 0., 1.);
    
    //影
    float t = RayMarch(rp + nv * SURF_DIST*2, lv);//現在のレイ位置からライト方向にレイを飛ばす
    if (t < len)
        brightness *= .7; //影なので暗くする
    
    return brightness;
}
float4 main(float4 i_pos : SV_POSITION, float2 i_uv : TEXCOORD) : SV_TARGET
{
    float3 ro = float3(0, 1, -5);//ray origin
    float3 rd = normalize(float3(i_uv, 1.));//ray direction
    
    //回転
    //ro.yz = mul(ro.yz, rot2D(-Time*0.1));
    //rd.yz = mul(rd.yz, rot2D(-Time*0.1));
    //ro.xz = mul(ro.xz, rot2D(Time*0.1));
    //rd.xz = mul(rd.xz, rot2D(Time*0.1));    
    
    float3 col = 0;//final color

    float t = RayMarch(ro, rd);//distance traveled
    
    //col = 1 - t / 15; //tの値視覚化
    //return float4(col, 1);
    
    float3 rp = ro + rd * t;//ray position
    col = Lighting(rp);
    return float4(col, 1);
    
    col = pow(col.x, .4545); //gamma correction
    return float4(col, 1);
}
