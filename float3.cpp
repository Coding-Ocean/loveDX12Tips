#include <cmath>
#include "float3.h"
float3::float3() :x(0), y(0), z(0) {

}
float3::float3(float x, float y, float z) {
    this->x = x;    this->y = y;    this->z = z;
}
void float3::set(float x, float y, float z) {
    this->x = x;    this->y = y;    this->z = z;
}
float3 float3::operator-(const float3& v) const {
    return float3(x - v.x, y - v.y, z - v.z);
}
float3 float3::operator+(const float3& v) const {
    return float3(x + v.x, y + v.y, z + v.z);
}
float3 float3::operator*(float f) const {
    return float3(x * f, y * f, z * f);
}
float3 float3::operator/(float f) const {
    return float3(x / f, y / f, z / f);
}
float3 float3::operator-() const {
    return float3(-x, -y, -z);
}
void float3::operator+=(const float3& v) {
    x += v.x;   y += v.y;   z += v.z;
}
void float3::operator-=(const float3& v) {
    x -= v.x;    y -= v.y;    z -= v.z;
}
void float3::operator*=(float f) {
    x *= f;    y *= f;    z *= f;
}
void float3::operator/=(float f) {
    x /= f;    y /= f;    z /= f;
}
float float3::sqMag() const {
    return x * x + y * y + z * z;
}
float float3::magSq() const {
    return x * x + y * y + z * z;
}
float float3::mag() const {
    return sqrtf(x * x + y * y + z * z);
}
float3 float3::setMag(float mag) {
    float len = sqrtf(x * x + y * y + z * z);
    if (len > 0) {
        mag /= len;
        x *= mag;
        y *= mag;
        z *= mag;
    }
    return float3(x, y, z);
}
float3 float3::limmit(float maxMag) {
    float len = sqrtf(x * x + y * y + z * z);
    if (len > maxMag) {
        maxMag /= len;
        x *= maxMag;
        y *= maxMag;
        z *= maxMag;
    }
    return float3(x, y, z);
}
float3 float3::normalize() {
    float len = sqrtf(x * x + y * y + z * z);
    if (len > 0) {
        x /= len;
        y /= len;
        z /= len;
    }
    return float3(x, y, z);
}
float float3::dot(const float3& v) const {
    return x * v.x + y * v.y + z * v.z;
}
float3 float3::cross(const float3& v) const {
    return float3(
        y * v.z - z * v.y,
        z * v.x - x * v.z,
        x * v.y - y * v.x
    );
}
float float3::crossZ(const float3& v) const {
    return x * v.y - y * v.x;
}
float float3::crossY(const float3& v) const {
    return x * v.z - z * v.x;
}

//‚RŸŒ³ƒxƒNƒgƒ‹‚Ìˆê”ÊŠÖ”-------------------------------------------------------
float3 operator*(float f, const float3& v) {
    return float3(f * v.x, f * v.y, f * v.z);
}
//”ñ”j‰ónoramalize
float3 normalize(const float3& a) {
    float l = sqrtf(a.x * a.x + a.y * a.y + a.z * a.z);
    if (l > 0) {
        return float3(a.x / l, a.y / l, a.z / l);
    }
    return a;
}
float dot(const float3& a, const float3& b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}
float3 cross(const float3& a, const float3& b) {
    return float3(
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x
    );
}
float crossZ(const float3& a, const float3& b) {
    return a.x * b.y - a.y * b.x;
}
float crossY(const float3& a, const float3& b) {
    return a.x * b.z - a.z * b.x;
}



