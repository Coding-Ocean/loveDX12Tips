#include"framework.h"

void gmain()
{
	window("WINDOW", 1280, 720);

	float toRad = 3.1415926f / 180;

	int img = loadImage("assets/penguin1.png");
	float px = width / 2;
	float py = height / 2;
	float rad = 0;
	float scale = 1.0f;
	
	hideCursor();
	initDeltaTime();
	while (!quit())
	{
		//更新------------------------------------------------
		setDeltaTime();
		getInputState();
		if (isTrigger(KEY_ESC)) closeWindow();

		//image
		if (isPress(KEY_D))px += 800 * delta;
		if (isPress(KEY_A))px -= 800 * delta;
		if (isPress(MOUSE_MBUTTON)) {
			px += mouseVx;
			py -= mouseVy;
		}
		if (isTrigger(MOUSE_LBUTTON))rad += 90 * toRad;// *delta;
		if (isTrigger(MOUSE_RBUTTON))rad -= 90 * toRad;// *delta;
		scale += 0.1f * mouseWheel;//マウスホィールはwindow.cppに定義してある
		float mx = mouseX;
		float my = mouseY;
		
		//arrow
		float sx = px;
		float sy = py;
		float vx = mx - sx;
		float vy = my - sy;
		float len = sqrtf(vx * vx + vy * vy);
		vx /= len;
		vy /= len;
		len -= 70;
		float ex = sx + vx * len;
		float ey = sy + vy * len;
		float arrowLen = 20, arrowRad = 40;

		//描画------------------------------------------------
		beginMsaaRender();
		
		image(img, px, py, rad, scale, scale);
		image(img, mx, my, 0, 0.3f, 0.3f);
		stroke(1, 0.4f, 0.4f);
		strokeWeight(10);
		arrow(sx, sy, ex, ey, arrowLen, arrowRad);

		fontSize(30);
		print("delta time:%.3f", delta);
		print("mx:%.0f,my:%.0f", mx, my);

		endMsaaRender();
	}
}
