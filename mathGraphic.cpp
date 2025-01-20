#include"graphic.h"
#include"window.h"
#include"input.h"
#include "mathGraphic.h"

//原点とする座標(スクリーン座標)
static float Ox, Oy;
//1とする大きさ
static float Scl;

void setAxis(float ox, float oy, float scl)
{
	Ox = ox;
	Oy = oy;
	Scl = scl;
}
void mathAxis()
{
	//ｘ、ｙ軸
	strokeWeight(2);
	line(0, Oy, baseWidth(), Oy);
	line(Ox, 0, Ox, baseHeight());

	//原点からnum個のメモリを描く
	float l = 0.05f * Scl;
	int num, i;
	num = (baseWidth() - Ox) / Scl;
	for (i = 1; i <= num; i++) {
		float x = Ox + Scl * i;
		line(x, Oy - l, x, Oy + l);
	}
	num = Ox / Scl;
	for (i = 1; i <= num; i++) {
		float x = Ox + Scl * -i;
		line(x, Oy - l, x, Oy + l);
	}
	num = (baseHeight() - Oy) / Scl;
	for (i = 1; i <= num; i++) {
		float y = Oy + Scl * i;
		line(Ox - l, y, Ox + l, y);
	}
	num = Oy / Scl;
	for (i = 1; i <= num; i++) {
		float y = Oy + Scl * -i;
		line(Ox - l, y, Ox + l, y);
	}
}
void mathCircle(float x, float y, float diameter)
{
	//スクリーン座標に変換
	x = Ox + Scl * x;
	y = Oy - Scl * y;
	diameter *= Scl;
	circle(x, y, diameter);
}
void mathLine(float sx, float sy, float ex, float ey)
{
	//スクリーン座標に変換
	sx = Ox + Scl * sx;
	sy = Oy - Scl * sy;
	ex = Ox + Scl * ex;
	ey = Oy - Scl * ey;

	line(sx, sy, ex, ey);
}
void mathArrow(float sx, float sy, float ex, float ey, float size)
{
	//スクリーン座標に変換
	sx = Ox + Scl * sx;
	sy = Oy - Scl * sy;
	ex = Ox + Scl * ex;
	ey = Oy - Scl * ey;
	size *= Scl;
	arrow(sx, sy, ex, ey, size, 30);
}
void mathGraph(float (*f)(float), float inc)
{
	float maxX = (baseWidth() - Ox) / Scl + inc;
	for (float x = 0; x < maxX; x += inc) {
		mathLine(x, f(x), x + inc, f(x + inc));
	}
	float minX = -Ox / Scl - inc;
	for (float x = 0; x > minX; x -= inc) {
		mathLine(x, f(x), x - inc, f(x - inc));
	}
}
float getMathMouseX()
{
	return (getMouseX() - Ox) / Scl;
}
float getMathMouseY()
{
	return -(getMouseY() - Oy) / Scl;
}
void mathStrokeWeight(float sw)
{
	sw *= Scl;
	strokeWeight(sw);
}
