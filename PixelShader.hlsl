#include "header.hlsli"

#define AA 2

//------------------------------------------------------------------
// ellipsoid SDF approximation
// https://iquilezles.org/articles/ellipsoids/
float sdEllipsoid(in float3 p, in float3 r)
{
    float k0 = length(p / r);
    float k1 = length(p / (r * r));
    return k0 * (k0 - 1.0) / k1;
}
float sdSphere(in float3 p, in float r)
{
    return length(p) - r;
}//------------------------------------------------------------------

float2 map(in float3 p)
{
    // sphere
    float2 d0 = float2(sdSphere(p - float3(0, 0, -0.5), 0.3), 0.75);
    // ellipsoid
    float2 d1 = float2(sdEllipsoid(p - float3(0, 0, 0), float3(0.2, 0.3, 0.1)), 1.0);
    // plane
    float2 d2 = float2(p.y + 0.3, 2.0);
    
    float2 d = (d0.x < d1.x) ? d0 : d1;
    return (d.x < d2.x) ? d : d2;
}

// https://iquilezles.org/articles/nvscene2008/
float2 castRay(in float3 ro, in float3 rd)
{
    float m = 0.0;
    float t = 0.0;
    const float tmax = 20.0;
    for (int i = 0; i < 256 && t < tmax; i++)
    {
        float2 h = map(ro + rd * t);
        if (h.x < 0.001)
            break;
        m = h.y;
        t += h.x;
    }

    return (t < tmax) ? float2(t, m) : float2(0.0,0.0);
}

//ambient occlusion
// https://iquilezles.org/articles/nvscene2008/
float calcAO(in float3 pos, in float3 nor)
{
    float occ = 0.0;
    float sca = 1.0;
    for (int i = 0; i < 5; i++)
    {
        float hr = 0.01 + 0.12 * float(i) / 4.0;
        float3 aopos = nor * hr + pos;
        float dd = map(aopos).x;
        occ += (hr - dd) * sca;
        sca *= 0.95;
    }
    return clamp(1.0 - 2.0 * occ, 0.0, 1.0);
}

// https://iquilezles.org/articles/rmshadows/
float calcSoftshadow(in float3 ro, in float3 rd)
{
    float res = 1.0;
    float t = 0.01;
    for (int i = 0; i < 256; i++)
    {
        float h = map(ro + rd * t).x;
        res = min(res, smoothstep(0.0, 1.0, 8.0 * h / t));
        t += clamp(h, 0.006, 0.02);
        if (res < 0.001 || t > 5.0)
            break;
    }
    return clamp(res, 0.0, 1.0);
}

// https://iquilezles.org/articles/normalsSDF/
float3 calcNormal(in float3 pos)
{
    float2 e = float2(1.0, -1.0) * 0.5773 * 0.0005;
    return normalize( e.xyy * map(pos + e.xyy).x +
					  e.yyx * map(pos + e.yyx).x +
					  e.yxy * map(pos + e.yxy).x +
					  e.xxx * map(pos + e.xxx).x);
}
 
// https://iquilezles.org/articles/checkerfiltering
float checkersGradBox(in float2 p)
{
    // filter kernel
    float2 w = fwidth(p) + 0.001;
    // analytical integral (box filter)
    float2 i = 2.0 * (abs(frac((p - 0.5 * w) * 0.5) - 0.5) - abs(frac((p + 0.5 * w) * 0.5) - 0.5)) / w;
    // xor pattern
    return 0.5 - 0.5 * i.x * i.y;
}

float3 render(in float3 ro, in float3 rd)
{
    float3 col = float3(0.0, 0.0, 0.0);
    
    float2 res = castRay(ro, rd);

    // floor
    if (res.y < 0.5)
        return col;
    
    // material        
    float t = res.x;
    float3 pos = ro + t * rd;
    float3 nor; //normal
    float occ; //occlusion

    if (res.y > 1.5)
    {
        nor = float3(0.0, 1.0, 0.0);
        occ = 1.0;
        col = 0.05 * float3(1.0, 1.0, 1.0);
        col *= 0.7 + 0.3 * checkersGradBox(pos.xz * 1.0);
    }
    else if (res.y >= 1.0)
    {
        nor = calcNormal(pos);
        occ = 0.5 + 0.5 * nor.y;
        col = float3(0.1, 0.1, 0.1);
    }
    else
    {
        nor = calcNormal(pos);
        occ = 0.5 + 0.5 * nor.y;
        col = float3(0.1, 0.05, 0.05);
    }

    // lighting
    occ *= calcAO(pos, nor);

    float3 lig = normalize(float3(0, 0.5, 1)); //z,y,x
    float3 hal = normalize(lig - rd);
    float amb = clamp(0.5 + 0.5 * nor.y, 0.0, 1.0);
    float dif = clamp(dot(nor, lig), 0.0, 1.0);
    float bac = clamp(dot(nor, normalize(float3(-lig.x, 0.0, -lig.z))), 0.0, 1.0) * clamp(1.0 - pos.y, 0.0, 1.0);

    float sha = calcSoftshadow(pos, lig);
    sha = sha * sha;

    float spe = pow(clamp(dot(nor, hal), 0.0, 1.0), 36.0) *
                    dif * sha *
                    (0.04 + 0.96 * pow(clamp(1.0 + dot(hal, rd), 0.0, 1.0), 5.0));
    col *= 5.0;
    col *= float3(0.2, 0.3, 0.4) * amb * occ + 1.6 * float3(1.0, 0.9, 0.75) * dif * sha;
    col += float3(2.8, 2.2, 1.8) * spe * 3.0;
    
    return col;
}

float4 main(float4 i_pos : SV_POSITION, float2 i_uv : TEXCOORD) : SV_TARGET
{
    // camera	
    float3 ro = float3(2.0 * sin(0.2 * iTime), 0.2, 2.0 * cos(0.2 * iTime));
    float3 ta = float3(0.0, 0.0, 0.0);
    // camera-to-world transformation
    float3 cw = normalize(ta - ro);
    float3 cu = normalize(cross(cw, float3(0.0, 1.0, 0.0)));
    float3 cv = (cross(cu, cw));

    // render
    float3 tot = float3(0.0, 0.0, 0.0); //total color
#if AA>1
    //AAが2以上の場合は、サブピクセルをAA*AA分だけサンプリング(加算)して、その平均を取る
    for (int m = 0; m < AA; m++)
    {
        for (int n = 0; n < AA; n++)
        {
            // pixel coordinates
            float2 o = float2(float(m), float(n)) / float(AA) - 0.5;
            float2 fc = i_uv + o;
#else    
            float2 fc = i_uv;
#endif
            float2 p = (2.0 * fc - iResolution.xy) / iResolution.y;

            // ray direction
            float3 rd = normalize(p.x * cu + p.y * cv + 2.0 * cw);

            // render	
            float3 col = render(ro, rd);

		    // gamma (yes, before accumulation)
            col = pow(col, float3(.4545, .4545, .4545));

            tot += col;
#if AA>1
        }
    }
    tot /= float(AA * AA);
#endif
    
    return float4(tot, 1.0);
}