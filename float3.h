#pragma once
struct float3 {
    float x, y, z;
    float3();
    float3(float x, float y, float z = 0);
    void set(float x, float y, float z = 0);
    float3 operator+(const float3& v) const;
    float3 operator-(const float3& v) const;
    float3 operator*(float f) const;
    float3 operator/(float f) const;
    float3 operator-() const;
    void operator+=(const float3& v);
    void operator-=(const float3& v);
    void operator*=(float f);
    void operator/=(float f);
    float sqMag() const;
    float magSq() const;//‹@”\‚ÍsqMag‚Æ“¯‚¶
    float mag() const;
    float3 setMag(float mag);
    float3 limmit(float maxMag);
    float3 normalize();
    float dot(const float3& v)const;
    float3 cross(const float3& v)const;
    float crossZ(const float3& v)const;//ŠOÏZ¬•ª‚Ì‚İŒvZ(‚Q‚c—p)
    float crossY(const float3& v)const;//ŠOÏY¬•ª‚Ì‚İŒvZ(‚Q‚c—p)
};

float3 operator* (float f, const float3& v);// float3 = float * float3
float3 normalize(const float3& a);
float dot(const float3& a, const float3& b);
float3 cross(const float3& a, const float3& b);
float crossZ(const float3& a, const float3& b);
float crossY(const float3& a, const float3& b);

