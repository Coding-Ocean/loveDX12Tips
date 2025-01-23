#pragma once
#define mathMouseX getMathMouseX()
#define mathMouseY getMathMouseY()
void mathStrokeWeight(float sw);
void setAxis(float ox, float oy, float scale);
void mathAxis();
void mathPoint(float px, float py);
void mathLine(float sx, float sy, float ex, float ey);
void mathArc(float ax, float ay, float bx, float by, float radius);
void mathArrow(float sx, float sy, float ex, float ey, float len = 0.1f, float deg = 30);
void mathCircle(float x, float y, float diameter);
void mathGraph(float (*f)(float), float inc = 0.1f);
void mathText(const char* str, float px, float py);
float getMathMouseX();
float getMathMouseY();