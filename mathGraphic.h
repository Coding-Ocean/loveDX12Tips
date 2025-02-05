#pragma once
#define mathMouseX getMathMouseX()
#define mathMouseY getMathMouseY()
void mathStrokeWeight(float sw);
void mathSetAxis(float ox, float oy, float scale);
void mathAxis();
void mathPoint(float px, float py);
void mathLine(float sx, float sy, float ex, float ey);
void mathArc(float ax, float ay, float bx, float by, float radius);
void mathArrow(float sx, float sy, float ex, float ey, float len = 0.1f, float deg = 30);
void mathCircle(float x, float y, float diameter);
void mathGraph(float (*f)(float), float inc = 0.1f);
void mathImage(int textureIdx, float px, float py, float rad = 0, float sx = 1, float sy = 1);
void mathText(const char* str, float px, float py);
void mathText(float px, float py, const char* format, ...);
float getMathMouseX();
float getMathMouseY();