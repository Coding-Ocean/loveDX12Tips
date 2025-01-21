#include"framework.h"

void gmain()
{
#if 0
	window("WINDOW", 720, 720);
#else
	window("WINDOW", 720, 720, full);
#endif

	int img = loadImage("assets/penguin1.png");
	float px = width / 2;
	float py = height / 2;
	float rad = 0;
	float scale = 1.0f;
	float moveSpeed = 800;
	float toRad = 3.1415926f / 180;
	float angleSpeed = 90 * toRad;
	float mx;
	float my;
	//hideCursor();
	initDeltaTime();
	while (!quit())
	{
		//更新------------------------------------------------
		setDeltaTime();
		getInputState();
		if (isTrigger(KEY_ESC)) closeWindow();

		//image
		if (isPress(KEY_D))px += moveSpeed * delta;
		if (isPress(KEY_A))px -= moveSpeed * delta;
		if (isPress(KEY_W))py -= moveSpeed * delta;
		if (isPress(KEY_S))py += moveSpeed * delta;
		if (isPress(MOUSE_LBUTTON))rad += angleSpeed * delta;
		if (isPress(MOUSE_RBUTTON))rad -= angleSpeed * delta;
		if (isPress(MOUSE_MBUTTON)) {
			px += mouseVx;
			py -= mouseVy;
		}		
		scale += 0.1f * mouseWheel;//マウスホィールはwindow.cppに定義してある
		mx = mouseX;
		my = mouseY;
		//arrow
		float sx = px;
		float sy = py;
		float vx = mx - sx;
		float vy = my - sy;
		float len = sqrtf(vx * vx + vy * vy);
		vx /= len;
		vy /= len;
		len -= 76;
		float ex = sx + vx * len;
		float ey = sy + vy * len;
		float arrowLen = 30, arrowRad = 30;

		//描画------------------------------------------------
		beginMsaaRender();
		//background rect
		fill(0.88f, 0.88f, 0.88f);
		stroke(1, 0.4f, 0.4f);
		strokeWeight(10);
		rectModeCorner();
		rect(0, 0, width, height);
		//charactors
		rectModeCenter();
		image(img, px, py, rad, scale, scale);
		image(img, mx, my, 0, 0.3f, 0.3f);
		fill(0, 0, 0, 0);
		circle(mx, my, 153);
		arrow(sx, sy, ex, ey, arrowLen, arrowRad);
		//info
		fontSize(30);
		print("deltaTime:%.3f", delta);
		print("mx:%.0f,my:%.0f", mx, my);
		//present
		endMsaaRender();
	}
}
