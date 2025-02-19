#pragma once
struct float2 {
    float x, y;
    float2();
    float2(float x, float y);
    void set(float x, float y);
    float2 operator+(const float2& v) const;
    float2 operator-(const float2& v) const;
    float2 operator*(float f) const;
    float2 operator/(float f) const;
    float2 operator-() const;
    void operator+=(const float2& v);
    void operator-=(const float2& v);
    void operator*=(float f);
    void operator/=(float f);
    float sqMag() const;
    float magSq() const;//機能はsqMagと同じ
    float mag() const;
    float2 setMag(float mag);
    float2 limmit(float maxMag);
    float2 normalize();
    float dot(const float2& v)const;
    float crossZ(const float2& v)const;//外積Z成分のみ計算
};

float2 operator* (float f, const float2& v);// float2 = float * float2
float2 normalize(const float2& a);
float dot(const float2& a, const float2& b);
float crossZ(const float2& a, const float2& b);//外積Z成分のみ計算

