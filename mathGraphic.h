#pragma once
#define mathMouseX getMathMouseX()
#define mathMouseY getMathMouseY()
void mathStrokeWeight(float sw);
void setAxis(float ox, float oy, float scale);
void mathAxis();
void mathCircle(float x, float y, float diameter);
void mathLine(float sx, float sy, float ex, float ey);
void mathArrow(float sx, float sy, float ex, float ey, float size=0.2f);
void mathGraph(float (*f)(float), float inc = 0.1f);
float getMathMouseX();
float getMathMouseY();