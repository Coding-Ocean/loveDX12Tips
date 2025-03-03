#include"graphic.h"
#include"window.h"
#include"input.h"
#include "mathGraphic.h"

//原点とする座標(スクリーン座標)
static float Ox, Oy;
//1とする大きさ
static float Scl;

void mathSetAxis(float ox, float oy, float scl)
{
	Ox = ox;
	Oy = oy;
	Scl = scl;
}
void mathAxis()
{
	//ｘ、ｙ軸
	line(0, Oy, baseWidth(), Oy);
	line(Ox, 0, Ox, baseHeight());

	//原点からnum個のメモリを描く
	float l = 0.03f * Scl;
	int num, i;
	num = int((baseWidth() - Ox) / Scl);
	for (i = 1; i <= num; i++) {
		float x = Ox + Scl * i;
		line(x, Oy - l, x, Oy + l);
	}
	num = int(Ox / Scl);
	for (i = 1; i <= num; i++) {
		float x = Ox + Scl * -i;
		line(x, Oy - l, x, Oy + l);
	}
	num = int((baseHeight() - Oy) / Scl);
	for (i = 1; i <= num; i++) {
		float y = Oy + Scl * i;
		line(Ox - l, y, Ox + l, y);
	}
	num = int(Oy / Scl);
	for (i = 1; i <= num; i++) {
		float y = Oy + Scl * -i;
		line(Ox - l, y, Ox + l, y);
	}
}
void mathPoint(float x, float y)
{
	//スクリーン座標に変換
	x = Ox + Scl * x;
	y = Oy - Scl * y;
	point(x, y);
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
void mathArrow(float sx, float sy, float ex, float ey, float len, float deg)
{
	//スクリーン座標に変換
	sx = Ox + Scl * sx;
	sy = Oy - Scl * sy;
	ex = Ox + Scl * ex;
	ey = Oy - Scl * ey;
	len *= Scl;
	arrow(sx, sy, ex, ey, len, deg);
}
void mathArc(float ax, float ay, float bx, float by, float radius)
{
	ax *= Scl; ay *= Scl; bx *= Scl; by *= Scl; radius *= Scl;
	arc(Ox, Oy, Ox + ax, Oy - ay, Ox + bx, Oy - by, radius);
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
void mathImage(int textureIdx, float px, float py, float rad, float sx, float sy)
{
	px = Ox + Scl * px;
	py = Oy - Scl * py;
	image(textureIdx, px, py, rad, sx, sy);
}
void mathText(const char* str, float px, float py)
{
	px = Ox + Scl * px;
	py = Oy - Scl * py;
	text(str, px, py);
}
void mathText(float px, float py, const char* format, ...)
{
	char str[256];
	va_list args;
	va_start(args, format);
	vsprintf_s(str, format, args);
	va_end(args);
	mathText(str, px, py);
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
