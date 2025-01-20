#define A_
#ifdef A_
#include<cmath>
#include"framework.h"
#include"mathGraphic.h"

void gmain()
{
#if 0
	int numDescriptors = 3000;
	window("Math", 1280, 720, win, numDescriptors);
#else
	int numDescriptors = 5000;
	window("Math", 1920, 1080, full, numDescriptors);
#endif
	//原点の位置をスクリーン座標で指定する
	float ox = width / 2;
	float oy = height / 2;
	//１とする大きさをドット数で指定する
	float scale = 100;
	//mouse x y
	float mx;
	float my;
	float rad;

	initDeltaTime();
	while (!quit())
	{
		//更新------------------------------------------------
		setDeltaTime();
		getInputState();
		if (isTrigger(KEY_ESC)) closeWindow();

		//math mouse
		mx = mathMouseX;
		my = mathMouseY;

		//描画------------------------------------------------
		beginMsaaRender();
		backgroundRect(0.12f, 0.1f, 0.1f);

		//set math axis
		if (isPress(MOUSE_MBUTTON)) {
			ox += mouseVx;
			oy -= mouseVy;
		}
		scale += 10 * mouseWheel;
		if (scale <= 50)scale = 50;
		setAxis(ox, oy, scale);

		//
		mathStrokeWeight(0.03);
		stroke(1.0f, 0.5f, 0.5f);
		mathArrow(0, 0, mx, my, 0.1f);
		stroke(0.9f, 0.9f, 0.9f);
		mathAxis();

		//info
		fontSize(30);
		fontColor(0.5f, 0.5f, 0.5f);
		print("numConstants:%d", numConstants());
		print("deltaTime:%.3f", delta);
		print("mathMouseX:%.2f", mx);
		print("mathMouseY:%.2f", my);
		print("mouseX:%.2f", mouseX);
		print("mouseY:%.2f", mouseY);
		print(" ");
		rad = atan2(my, mx);
		if (my < 0)rad += 3.1415926f * 2;
		print("atan2:%.2f", rad*180/3.1415926f);

		//present
		endMsaaRender();
	}
}
#endif
#ifdef B_
#include<cmath>
#include"framework.h"
#include"mathGraphic.h"

float quadratic(float x)
{
	return x * x;
}
float minus_cos(float x)
{
	return -cos(x);
}

void gmain()
{
#if 0
	int numDescriptors = 3000;
	window("Math", 1280, 720, win, numDescriptors);
#else
	int numDescriptors = 5000;
	window("Math", 1920, 1080, full, numDescriptors);
#endif
	//原点の位置
	float ox = width / 2;
	float oy = height / 2;
	//１とする大きさ
	float scale = width / 13;
	//mouse x y
	float mx;
	float my;
	//circle
	float diameter = 0.1f;
	float minX = -ox / scale;
	float maxX = 0;
	float px = minX;
	float vx = 1.5f;
	initDeltaTime();
	while (!quit())
	{
		//更新------------------------------------------------
		setDeltaTime();
		getInputState();
		if (isTrigger(KEY_ESC)) closeWindow();

		//math mouse
		mx = mathMouseX;
		my = mathMouseY;

		//描画------------------------------------------------
		beginMsaaRender();

		//set math axis
		if (isPress(MOUSE_MBUTTON)) {
			ox += mouseVx;
			oy -= mouseVy;
		}
		scale += 10 * mouseWheel;
		if (scale <= 50)scale = 50;
		setAxis(ox, oy, scale);

		//function graph and move circle
		minX = -ox / scale;
		maxX = (width - ox) / scale;
		px += vx * delta;
		if (px > maxX)px = minX;
		mathStrokeWeight(0.03);
		fill(0, 0, 0, 0);
		stroke(1, 0.5f, 0.5f);
		mathGraph(sin);
		mathCircle(px, sin(px), diameter);
		stroke(0.5f, 0.5f, 1.0f);
		mathCircle(px, cos(px), diameter);
		mathGraph(cos);
		stroke(0.5f, 1, 0.5f);
		mathCircle(px, quadratic(px), diameter);
		mathGraph(quadratic);
		stroke(0.9f, 0.9f, 0.9f);
		mathAxis();

		//info
		fontSize(30);
		fontColor(0.5f, 0.5f, 0.5f);
		print("numConstants:%d", numConstants());
		print("deltaTime:%.3f", delta);
		print("mouseX:%.2f", mx);
		print("mouseY:%.2f", my);

		//present
		endMsaaRender();
	}
}
#endif