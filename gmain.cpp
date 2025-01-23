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
	//cursor image
	int img = loadImage("assets/cursor.png");
	//axis
	//　原点の位置をスクリーン座標で指定する
	float ox = width / 2;
	float oy = height / 2;
	//　１とする大きさをドット数で指定する
	float scale = width / 6;
	//vector a
	float ax=1.5f;
	float ay=0;
	float al = sqrt(ax * ax + ay * ay);//length
	float anx = ax / al;//normalized x
	float any = ay / al;//normalized y
	//vector b
	float bx;
	float by;
	float bnx;//normalized x
	float bny;//normalized y
	//radian for rotate
	float rad = 0;
	//distance from vector tip to text
	float dist = 0.15f;
	initDeltaTime();
	while (!quit())
	{
		setDeltaTime();
		getInputState();
		if (isTrigger(KEY_ESC)) closeWindow();
		//clear
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
		//change vector a.
		if (isTrigger(MOUSE_LBUTTON)) {
			ax = mathMouseX;
			ay = mathMouseY;
			al = sqrt(ax * ax + ay * ay);//blをradiusとする
			anx = ax / al;
			any = ay / al;
		}
		//rotate vector b
		bnx = cos(rad);
		bny = sin(rad);
		rad += 90 * 3.1415926f / 180 * delta;
		bx = bnx * al;//vector aと同じ大きさにする
		by = bny * al;
		//circle
		mathStrokeWeight(0.05f);
		stroke(0.6f, 0.6f, 1.0f);
		mathCircle(0, 0, al * 2);
		//setup text font
		fontRectModeCenter();
		fontSize(25);
		fontColor(1, 1, 1);
		//arrow a
		stroke(1.0f, 0.5f, 0.5f);
		mathArrow(0, 0, ax, ay);
		mathText("a", ax + anx * dist, ay + any * dist);
		//arrow b
		stroke(1.0f, 1.0f, 0.6f);
		mathArrow(0, 0, bx, by);
		mathText("b", bx + bnx * dist, by + bny * dist);
		//arc
		stroke(0.5f, 0.5f, 0.5f);
		mathArc(ax, ay, bx, by, al / 6);
		//axis
		stroke(0.6f, 0.6f, 0.6f);
		mathAxis();
		//cursor
		mathImage(img, mathMouseX, mathMouseY);
		//info
		fontSize(20);
		fontRectModeCorner();
		print("numConstants:%d", numConstants());
		print("deltaTime:%.3f", delta);
		print("mathMouseX:%.2f", mathMouseX);
		print("mathMouseY:%.2f", mathMouseY);
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
	hideCursor();
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
		mathStrokeWeight(0.03f);

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
		mathStrokeWeight(0.1f);
		


		//info
		fontSize(30);
		fontColor(0.5f, 0.5f, 0.5f);
		print("numConstants:%d", numConstants());
		print("deltaTime:%.3f", delta);
		print("mathMouseX:%.2f", mathMouseX);
		print("mathMouseY:%.2f", mathMouseY);

		//present
		endMsaaRender();
	}
}
#endif
#ifdef C_
//math系関数を使用しないでつくる。
//デカルト座標で考え、スクリーン座標に変換して描画する。
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
	//cursor image
	int img = loadImage("assets/cursor.png");
	//axis
	//　原点の位置をスクリーン座標で指定する
	float ox = width / 2;
	float oy = height / 2;
	//　１とする大きさをドット数で指定する
	float scale = width / 6;
	//vector a
	float ax=100;
	float ay=0;
	//normalize vector a
	float al=sqrt(ax*ax+ay*ay);
	float anx = ax / al;
	float any = ay / al;
	//normalized vector b
	float bx = 0;
	float by = 0;
	float bnx = 0;
	float bny = 0;
	//rotation radian
	float rad = 0;
	//文字を矢印方向にずらす距離
	float dist = 15;
	initDeltaTime();
	while (!quit())
	{
		setDeltaTime();
		getInputState();
		if (isTrigger(KEY_ESC)) closeWindow();
		//clear
		beginMsaaRender();
		//フルスクリーンの時、rectでウィンドウの枠線を引く
		noFill();//今回は塗りつぶさないが、もちろんfillで塗りつぶしてもよい。
		stroke(0.5f, 0.5f, 0.5f);
		strokeWeight(2);
		rectModeCorner();
		rect(0, 0, width, height);
		//上がプラスの座標で考える
		//change vector a. 
		if (isTrigger(MOUSE_LBUTTON)) {
			ax = mouseX - ox;
			ay = -(mouseY - oy);
			al = sqrt(ax * ax + ay * ay);//alをradiusとする
			anx = ax / al;
			any = ay / al;
		}
		//rotate vector b
		rad += 90 * 3.1415926f / 180 * delta;
		bnx = cos(rad);
		bny = sin(rad);
		bx = bnx * al;
		by = bny * al;
		//ここからスクリーン座標で考える
		//circle
		strokeWeight(5);
		stroke(0.6f, 0.6f, 1.0f);
		circle(ox, oy, al * 2);
		//setup text font
		fontRectModeCenter();
		fontSize(25);
		fontColor(1, 1, 1);
		//arrow a
		stroke(1.0f, 1.0f, 0.6f);
		float x = ox + ax;
		float y = oy - ay;
		arrow(ox, oy, x, y);
		text("a", x + anx * dist, y - any * dist);
		//arrow b
		stroke(1.0f, 0.5f, 0.5f);
		x = ox + bx;
		y = oy - by;
		arrow(ox, oy, x, y);
		text("b", x + bnx * dist, y - bny * dist);
		//arc
		stroke(0.5f, 0.5f, 0.5f);
		arc(ox, oy, ox + anx, oy - any, ox + bnx, oy - bny, al / 6);
		//cursor
		image(img, mouseX, mouseY);
		//info
		fontSize(20);
		fontColor(0.5f, 0.5f, 0.5f);
		fontRectModeCorner();
		print("numConstants:%d", numConstants());
		print("deltaTime:%.3f", delta);
		print("mouseX:%.2f", mouseX);
		print("mouseY:%.2f", mouseY);
		print("ax:%f", ax);
		print("ay:%f", ay);
		//present
		endMsaaRender();
	}
}
#endif