#define A_
#ifdef A_
#include<cmath>
#include"framework.h"
#include"mathGraphic.h"
void gmain()
{
#if 0
	window("Math", 640, 640, win);
#else
	window("Math", 640, 640, full);
	//フルスクリーンモードで、
	//指定した幅と高さのアスペクト比と
	//ディスプレイ設定のアスペクト比が違う場合
	//実際のカーソル位置と使用するカーソル位置がずれるので、隠さないといけない。
	//数値的には問題ない。
	hideCursor();
#endif
	//原点の位置をスクリーン座標で指定する
	float ox = width / 2;
	float oy = height / 2;
	//１とする大きさをドット数で指定する
	float scale = width / 12;
	//mouse x y
	float mx;
	float my;
	float radius;
	initDeltaTime();
	while (!quit())
	{
		setDeltaTime();
		getInputState();
		if (isTrigger(KEY_ESC)) closeWindow();

		//描画------------------------------------------------
		beginMsaaRender();

		//フルスクリーンの時、rectでウィンドウの枠線を引く
		noFill();//今回は塗りつぶさないが、もちろんfillで塗りつぶしてもよい。
		stroke(0.5f, 0.5f, 0.5f);
		strokeWeight(2);
		rectModeCorner();
		rect(0, 0, width, height);
		
		//set math axis
		if (isPress(MOUSE_MBUTTON)) {
			ox += mouseVx;
			oy -= mouseVy;
		}
		scale += 10 * mouseWheel;
		if (scale <= 50)scale = 50;
		setAxis(ox, oy, scale);

		//mouse
		mx = mathMouseX;
		my = mathMouseY;
		radius = sqrt(mx * mx + my * my);
		//circle
		mathStrokeWeight(0.05f);
		stroke(0.5f, 0.5f, 1.0f);
		mathCircle(0, 0, radius * 2);
		//arrow
		stroke(1.0f, 0.5f, 0.5f);
		mathArrow(0, 0, mx, my);
		//axis
		stroke(0.6f, 0.6f, 0.6f);
		mathAxis();

		//info
		fontSize(30);
		fontColor(0.5f, 0.5f, 0.5f);
		print("numConstants:%d", numConstants());
		print("deltaTime:%.3f", delta);
		print("mathMouseX:%.2f", mx);
		print("mathMouseY:%.2f", my);

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
	window("Math", 720, 720, win);
#else
	window("Math", 720, 720, full);
#endif
	//原点の位置
	float ox = width / 2;
	float oy = height / 2;
	//１とする大きさ
	float scale = width / 13;
	//circle
	float diameter = 0.15f;
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

		//描画------------------------------------------------
		beginMsaaRender();

		//フルスクリーンの時、rectでウィンドウの枠線を引く
		noFill();//今回は塗りつぶさないが、もちろんfillで塗りつぶしてもよい。
		stroke(0.5f, 0.5f, 0.5f);
		strokeWeight(2);
		rectModeCorner();
		rect(0, 0, width, height);

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
		mathGraph(cos);
		mathCircle(px, cos(px), diameter);
		
		stroke(0.5f, 1, 0.5f);
		mathGraph(quadratic);
		mathCircle(px, quadratic(px), diameter);
		
		stroke(0.9f, 0.9f, 0.9f);
		mathAxis();

		//info
		fontSize(30);
		fontColor(0.5f, 0.5f, 0.5f);
		print("numConstants:%d", numConstants());
		print("deltaTime:%.3f", delta);

		//present
		endMsaaRender();
	}
}
#endif