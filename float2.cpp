#include <cmath>
#include "float2.h"
float2::float2() :x(0), y(0) {

}
float2::float2(float x, float y) {
    this->x = x;    this->y = y;
}
void float2::set(float x, float y) {
    this->x = x;    this->y = y;
}
float2 float2::operator-(const float2& v) const {
    return float2(x - v.x, y - v.y);
}
float2 float2::operator+(const float2& v) const {
    return float2(x + v.x, y + v.y);
}
float2 float2::operator*(float f) const {
    return float2(x * f, y * f);
}
float2 float2::operator/(float f) const {
    return float2(x / f, y / f);
}
float2 float2::operator-() const {
    return float2(-x, -y);
}
void float2::operator+=(const float2& v) {
    x += v.x;   y += v.y;
}
void float2::operator-=(const float2& v) {
    x -= v.x;    y -= v.y;
}
void float2::operator*=(float f) {
    x *= f;    y *= f;
}
void float2::operator/=(float f) {
    x /= f;    y /= f;
}
float float2::sqMag() const {
    return x * x + y * y;
}
float float2::magSq() const {
    return x * x + y * y;
}
float float2::mag() const {
    return sqrtf(x * x + y * y);
}
float2 float2::setMag(float mag) {
    float len = sqrtf(x * x + y * y);
    if (len > 0) {
        mag /= len;
        x *= mag;
        y *= mag;
    }
    return float2(x, y);
}
float2 float2::limmit(float maxMag) {
    float len = sqrtf(x * x + y * y);
    if (len > maxMag) {
        maxMag /= len;
        x *= maxMag;
        y *= maxMag;
    }
    return float2(x, y);
}
float2 float2::normalize() {
    float len = sqrtf(x * x + y * y);
    if (len > 0) {
        x /= len;
        y /= len;
    }
    return float2(x, y);
}
float float2::dot(const float2& v) const {
    return x * v.x + y * v.y;
}
float float2::crossZ(const float2& v) const {
    return x * v.y - y * v.x;
}

//‚QŸŒ³ƒxƒNƒgƒ‹‚Ìˆê”ÊŠÖ”-------------------------------------------------------
float2 operator*(float f, const float2& v) {
    return float2(f * v.x, f * v.y);
}
//”ñ”j‰ónoramalize
float2 normalize(const float2& a) {
    float l = sqrtf(a.x * a.x + a.y * a.y);
    if (l > 0) {
        return float2(a.x / l, a.y / l);
    }
    return a;
}
float dot(const float2& a, const float2& b) {
    return a.x * b.x + a.y * b.y;
}
float crossZ(const float2& a, const float2& b) {
    return a.x * b.y - a.y * b.x;
}



